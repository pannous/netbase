#include <cstdlib> // malloc, exit:
#include <sys/sem.h> // sudo ipcrm -M 0x69190
//#include <errno.h> // just use perror("blah")! Adds errorstr(errno) automagically
//#include <sys/types.h>
//#include <sys/ipc.h>
#include <sys/shm.h> // shared memory
#include <sys/mman.h>// memory map
#include <sys/stat.h> // mkdir
//#include <mmap.h> // mmap_read NOPE
//#include "mmap.h" // mmap_read NOPE


//Since using #include <stdlib.h> dumps all declared names in the global namespace,
//the preference should be to use #include <cstdlib>, unless you need compatibility with C
#include <string.h>

//#include <stdlib.h> // system(cmd)
#include <unistd.h> // getpid
#include <signal.h> // kill
#include <sys/stat.h>
#include <fcntl.h> // falloc, open


#include "util.hpp"
#include "init.hpp"
#include "relations.hpp"
#include "webserver.hpp"

// bool USE_MMAP = true; // memory map to file on systems with small RAM
bool USE_MMAP = false; // use shared memory

namespace patch {
	template<typename T>
	std::string to_string(const T &n) {
		std::ostringstream stm;
		stm << n;
		return stm.str();
	}
}


bool _allowWipe = false;

/* attach to the segment to get a pointer to it: */
//const void * shmat_root = (const void *) 0x101000000; // mac 64 bitpointer to it: */
#ifdef i386
const void * shmat_root = (const void *) 0x10000000; // just higher than system Recommendation
#else
//const void *shmat_root = (const void *) 0x0220000000;// java !
//const void *shmat_root = (const void *) 0x7f6000000000;
const void *shmat_root = (const void *) 0x600000000000;
#endif

bool virgin_memory = false;
#if defined(__FreeBSD__) or defined(__APPLE__)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
	int val; /* Value for SETVAL */
	struct semid_ds *buf; /* Buffer for IPC_STAT, IPC_SET */
	unsigned short *array; /* Array for GETALL, SETALL */
	struct seminfo *__buf; /* Buffer fTheor IPC_INFO
	 (Linux-specific) */
};



#endif

int semrm(key_t key, int id = 0) {
	union semun arg;
	id = semget(key, 0, 0);
	if (id == -1) return -1;
	return semctl(id, 0, IPC_RMID, arg);
}


// #include <execinfo.h>

static void full_write(int fd, const char *buf, size_t len) {
	while (len > 0) {
		ssize_t ret = write(fd, buf, len);

		if ((ret == -1) and (errno != EINTR))
			break;

		buf += (size_t) ret;
		len -= (size_t) ret;
	}
}

void print_backtrace(void) {
	static const char start[] = "-- NETBASE BACKTRACE -------\n";
	static const char end[] = "----------------------\n";

	void *bt[1024];
	int bt_size;
	char **bt_syms;
	int i;

#ifdef _EXECINFO_H
	bt_size = backtrace(bt, 1024);
	bt_syms = backtrace_symbols(bt, bt_size);
	full_write(STDERR_FILENO, start, strlen(start));
	for (i = 1; i < bt_size; i++) {
		size_t len = strlen(bt_syms[i]);
		full_write(STDERR_FILENO, bt_syms[i], len);
		full_write(STDERR_FILENO, "\n", 1);
	}
	full_write(STDERR_FILENO, end, strlen(end));
	free(bt_syms);
#else
	printf("compile with EXECINFO for backtrace");
#endif
}

// silent: in messages:
//Jun 3 15:53:30 507 abrt[13188]: abrtd is not running. If it crashed, /proc/sys/kernel/core_pattern contains a stale value, consider resetting it to 'core'
// better: create core dumps! enable:
// MAC echo "limit core unlimited" | sudo tee -a /etc/launchd.conf   ... see /cores ~/Library/Logs/DiagnosticReports
// LINUX ulimit -c unlimited
void signal_handler(int signum) {// handle SIGSEGV smoothly
	printf("Process %d got signal %d\n", getpid(), signum);
	print_backtrace();
	signal(signum, SIG_DFL);
	kill(getpid(), signum);
}

//long GB=1073741824;
void increaseShmMax() {
	p("please call ./increase-shared-memory.sh");
//	p("increase ShmMax");
//	system("./increase-shared-memory.sh");// kill 9 itself :(
//	return;
	//  sudo: no tty present and no askpass program specified in Xcode


#ifdef __APPLE__
	long mem=4*GB;
//	long mem=sizeOfSharedMemory;
	system((string("sudo sysctl -w kern.sysv.shmmax=")+std::to_string(mem)).data());
	system((string("sudo sysctl -w kern.sysv.shmall=")+std::to_string(mem/4096)).data());
#else
//	system((string("sudo sysctl -w kernel.shmmax=") + patch::to_string(sizeOfSharedMemory)).data());
//	system((string("sudo sysctl -w kernel.shmall=") + patch::to_string(sizeOfSharedMemory / 4096)).data());
#endif
	p("If you still cannot start netbase, decrease maxNodes in netbase.hpp");// or adjust shmmax, see clear-shared-memory.sh");
}

void releaseSharedMemory() {
	p("run as sudo or USE ./clear-shared-memory.sh ");
//	system("./clear-shared-memory.sh");// kill -9 self process :(
	system("ipcrm -M '0x69190'");
	system("ipcrm -M '0x69191'");
	system("ipcrm -M '0x69192'");
	system("ipcrm -M '0x69193'");
	system("ipcrm -M '0x69194'");
	system("ipcrm -M '0x69195'");
	system("ipcrm -M '0x69196'");
	increaseShmMax();
}

void detach_shared_memory() {
	// TODO (?) programmatically
	if (!_allowWipe) {
		p("AUTOMATIC MEMORY WIPE DISABLED!");
		p("If you cannot start netbase try:\n ./increase-shared-memory.sh and ./clear-shared-memory.sh");
		return;
	}
	increaseShmMax();
	releaseSharedMemory();
}

void *share_memory(key_t key, long long sizeOfSharedMemory, void *root, const void *desired) {
	if (root) {
//		pf("root_memory already attached! %p\n", root);
		return root;
	}
	/* make the key: */
	//	int key = 0x69190; //0x57020303;// #netbase ftok("netbase", 'RW');
	int shmid;
	//        virgin_memory=0;
	int READ_WRITE = 0666; //
	if ((shmid = shmget(key, sizeOfSharedMemory, READ_WRITE)) == -1) {
		ps("share_memory used for the first time");
		pf("requesting 0x%llx bytes ~ %lld MB \n", sizeOfSharedMemory, sizeOfSharedMemory / MB);
		virgin_memory = 1;
		if ((shmid = shmget(key, sizeOfSharedMemory, READ_WRITE | IPC_CREAT)) == -1) {
			semrm(key); // clean and try again
			if ((shmid = shmget(key, sizeOfSharedMemory, READ_WRITE | IPC_CREAT)) == -1) {
				pf("nodes: %ld\n", maxNodes);
				perror("share_memory failed!\nSize changed or NOT ENOUGH MEMORY??\n shmget");
				//			printf("try calling ./clear-shared-memory.sh\n");
				//			perror(strerror(errno)); <= ^^ redundant !!!
				printf("ipcclean and sudo ipcrm -M 0x69190 ... \n./clear-shared-memory.sh\n");
				detach_shared_memory();
				increaseShmMax();
			}
			if ((shmid = shmget(key, sizeOfSharedMemory, READ_WRITE | IPC_CREAT)) == -1) {
				perror("share_memory failed: shmget! Not enough memory?");
				exit(1);
			}
		}
	}
	root = (char *) shmat(shmid, desired, 0);
	if (root == 0 or root == (void *) (-1)) { //virgin_memory=1;
		ps("receiving other share_memory address");
		root = (char *) shmat(shmid, (const void *) 0, 0); //void
	}
	if (root == 0 or root == (void *) (-1)) {
		perror("share_memory failed: shmat! Not enough memory?");
		exit(1);
	}
//	Context* c=context; // getContext(node->context);
	if ((char *) root != (char *) desired) { // 64 BIT : %x -> %016llX
		printf("FYI: root_memory != desired shmat_root %p!=%p \n", root, desired);
//		fixPointers();
	}
	pf("share_memory at %p	size = %llX	max = %p\n", root, sizeOfSharedMemory, (char *) root + sizeOfSharedMemory);
	return root;
}

long getMemory() {
	//long phypz = sysconf(_SC_PHYS_PAGES);
	//long psize = sysconf(_SC_PAGE_SIZE);
	//return phypz*psize;
	return 0;
}

long GetAvailableMemory(void) {
	void *p, *q;
	long siz = 10000000;
	q = p = calloc(1, siz);
	while (q) {
		siz = siz * 1.2; // Can be more to speed up things
		q = realloc(p, siz);
		if (q) // infinite virtual mem :{ 31107287834197
			p = q;
	}
	free(p);
	return siz;
}

// modify char* in vivo / inline!

void initRootContext() {
	initContext(context_root);
	strcpy(context_root->name, "ROOT CONTEXT");
	context_root->nodes = (Node *) node_root + propertySlots;
	context_root->statements = (Statement *) statement_root;
	context_root->nodeNames = name_root;
	context->currentNameSlot = 0;
	context->nodeCount = 1;// 0 = ANY
	context->lastNode = 1;
	context->extrahashNr = 0;
	context->statementCount = 1;// 0 = ERROR
}

void checkRootContext() {
#ifdef WASM
	return;// only one new Context() ... ok
#endif
	Context *rootContext = (Context *) context_root;
	if (rootContext->nodeCount == 0) {
		p("STARTING WITH CLEAN MEMORY");
		initRootContext();
//    clearMemory();
		return;
	}
	if (context->nodeNames != name_root)
		fixPointers();
	rootContext->nodes = (Node *) node_root + propertySlots;;
	rootContext->statements = statement_root;
}


extern "C" void initSharedMemory(bool relations) {
	if (USE_MMAP) {
		p("USE_MMAP => NO initSharedMemory!");
		contexts = context_root = static_cast<Context *>(malloc(1000 * sizeof(Context)));
		context = &context_root[0];
		loadMemoryMaps();
		context = getContext();
		if (context->currentNameSlot > maxChars - 1000000)
			context->currentNameSlot = maxChars - 1000000;// hack for Relations in full graph
		if (relations and not hasWord("Synonym"))
			initRelations();
		return;
	}
//	signal(SIGSEGV, signal_handler); // handle SIGSEGV smoothly. USELESS for print_backtrace
	signal(SIGCHLD, SIG_IGN); // https://stackoverflow.com/questions/6718272/c-exec-fork-defunct-processes
//	print_backtrace();
	if (!relations)testing = true;
	//  if ((i = setjmp(try_context)) == 0) {// try once
	int key = 0x69190;
	char *root = (char *) shmat_root;
	long context_size = contextOffset;
	long node_size = maxNodes * sizeof(Node);
	long abstract_size = maxNodes * sizeof(Ahash) * 2;
	long long name_size = maxChars;
	long statement_size = maxStatements * sizeof(Statement);
//	node_root=&context_root[contextOffset];
//	p("abstract_root:");
	abstracts = (Ahash *) share_memory(key, abstract_size, abstracts, root);
	// ((char*) context_root) + context_size
//	p("name_root:");
	char *desiredAddress = ((char *) abstracts) + abstract_size;
	name_root = (char *) share_memory(key + 1, name_size, name_root, desiredAddress);
	//	p("node_root:");
	auto desired_node_root = name_root + name_size;// + 0x4e15c54800;// @jini WHY?
	node_root = (Node *) share_memory(key + 2, node_size, node_root, desired_node_root);
//	p("statement_root:");
	char *desiredRootS = ((char *) node_root) + node_size;
	statement_root = (Statement *) share_memory(key + 3, statement_size, statement_root, desiredRootS);
//	p("keyhash_root:");// for huge datasets ie freebase
//	short ns = sizeof(Node*); // ;
//	keyhash_root = (Node**) share_memory(key + 5, 1 * billion * ns, keyhash_root, ((char*) statement_root) + statement_size);
	//	freebaseKey_root=(int*) share_memory(key + 5, freebaseHashSize* sizeof(int), freebaseKey_root, ((char*) statement_root) + statement_size);

//  	p("context_root:");
	char *desiredRootC = ((char *) statement_root) + statement_size;
	context_root = (Context *) share_memory(key + 4, context_size, context_root, desiredRootC);
//	extrahash = (Ahash *) &abstracts[maxNodes]; // (((char*)abstract_root + abstractHashSize);
	contexts = (Context *) context_root;
	context = getContext(current_context);
	checkRootContext();
	if (relations) {
//		p(get(_clazz));
//		if(!checkNode(-9) and debug)
		initRelations();
		if (context->lastNode < 0)
			context->lastNode = wikidata_limit;
	}
}

void loadMemoryMaps();


void file_allocate(int fd, long size);

//static long shared_memory_2GB=2147483648;
//static long shared_memory_4GB=4294967296;
//static long shared_memory_6GB=6442450944;
//static long shared_memory_8GB=8589934592;
//static long shared_memory_16GB=17179869184;
//static long shared_memory_32GB=34359738368;
//static long shared_memory_64GB=68719476736;
void setMemoryLimit(long maxNodes0, long maxStatements0, long maxChars0) {
//	void setMemoryLimit(long maxNodes0,long maxStatements0=-1,long maxChars0=-1){
	if (maxNodes0 > 4294967296)error("sorry, 4 billion nodes is currently the limit of netbase");
	maxNodes = maxNodes0; /*max 32bit=4 billion! */
	if (maxStatements0 > 0) maxStatements = maxStatements0;
	else maxStatements = maxNodes * 2;// *10 = crude average of Statements per Node (yago:12!!)
	if (maxChars0 > 0) maxChars = maxChars0;
	else maxChars = maxNodes * averageNameLength;
	bytesPerNode = (sizeof(Node) + averageNameLength);//+ahashSize*2
	sizeOfSharedMemory = contextOffset + maxNodes * bytesPerNode + maxStatements * sizeof(Statement);
	initSharedMemory(true);
}

extern "C" void init(bool relations) {
	initSharedMemory(relations);
}


void fixStatementNodeIds(Context *context, Node *oldNodes) {
#ifndef explicitNodes
	return;
#else
	int max=context->statementCount; // maxStatements;
	for (int i=0; i < max; i++) {
		Statement* n=&context->statements[i];
		if (!checkStatement(n)) {
			showStatement(n);
			continue;
		}
		n->Subject=&context->nodes[n->subject];
		n->Predicate=&context->nodes[n->predicate];
		n->Object=&context->nodes[n->object];
	}
#endif
}

FILE *open_binary(cchar *file) {
//	const char *fullpath = (data_path + file).data(); AAARG out of scope => "" WTF!!
	printf("Opening File %s\n", (data_path + file).data());
	FILE *fp = fopen((data_path + file).data(), "rb");
	if (fp == NULL)perror("Error opening file");
	return fp;
}


// returns file-descriptor or 0
int open_map(const char *file0, long size) {
	const char *fullpath = concat(data_path.c_str(), file0);
	bool exists = file_exists(fullpath);
	errno = 0;
	int fd;
	int oflag = O_CREAT | O_RDWR;// | O_LARGEFILE;// O_WRONLY \| \| O_APPEND \|
	if (LIVE) {
		oflag = O_RDONLY;
		fd = open(fullpath, oflag);
	} else fd = open(fullpath, oflag, 0777);
//	bool grow = false;
	bool grow = true;// DANGER?
	long filesize = fileSize(fd);
	if (!exists or (filesize < size)){
		printf("GROWING %s from %ld to %ld bytes", file0, filesize, size);
		if(grow){file_allocate(fd,size);
		}
	}
	if (errno and errno != EEXIST) {
		p(fullpath);
		perror("FAILED fallocate ");
		errno = 0;
		return 0;
	}
	return fd;
}

void file_allocate(int fd, long size) {
#if defined(HAVE_POSIX_FALLOCATE)
    posix_fallocate(fd, 0, size);
#elif defined(XP_WIN)
    return PR_Seek64(aFD, aLength, PR_SEEK_SET) == aLength
    && 0 != SetEndOfFile((HANDLE)PR_FileDesc2NativeHandle(aFD));
#elif defined(XP_MACOSX)
    int fd = PR_FileDesc2NativeHandle(aFD);
  fstore_t store = {F_ALLOCATECONTIG, F_PEOFPOSMODE, 0, aLength};
  // Try to get a continous chunk of disk space
  int ret = fcntl(fd, F_PREALLOCATE, &store);
    if(-1 == ret){
    // OK, perhaps we are too fragmented, allocate non-continuous
    store.fst_flags = F_ALLOCATEALL;
    ret = fcntl(fd, F_PREALLOCATE, &store);
    if (-1 == ret)
      return false;
  }
  return 0 == ftruncate(fd, aLength);
#endif
}

void loadMemoryMaps() {
	p("Preparing mapped memory files, using some GB! This can take some minutes!");
	mkdir(data_path.c_str(), S_IRWXU | S_IRWXG);// user+group rwx
// MAP_UNINITIALIZED, MAP_SYNC in write maps
	int mode = PROT_READ | PROT_WRITE;
	if (LIVE)mode = PROT_READ;// no PROT_WRITE!
	int flags = MAP_SHARED;// |MAP_NORESERVE|MAP_NONBLOCK|MAP_FIXED;

	size_t contextSize = contextOffset * 2;// * sizeof(Context);
	size_t nameSize = maxChars;// * sizeof(char);
	size_t statementsSize = maxStatements * sizeof(Statement);
	size_t nodesSize = maxNodes * sizeof(Node);
	size_t abstractsSize = sizeof(Ahash) * maxNodes * 2;

	int fd;


	fd = open_map("abstracts.bin", abstractsSize);
	abstracts = (Ahash *) mmap((char *) shmat_root, abstractsSize, mode, flags, fd, 0);
	if (abstracts == MAP_FAILED)perror("abstracts.bin MAP_FAILED");
	close(fd);

	fd = open_map("names.bin", nameSize);
	name_root = (char *)mmap((char *) abstracts + abstractsSize, nameSize, mode, flags, fd, 0);
	close(fd);
	if (name_root == MAP_FAILED) {
		perror("names.bin MAP_FAILED ");
		exit(0);
	}

	fd = open_map("nodes.bin", nodesSize);
	node_root = (Node *) mmap(name_root + nameSize, nodesSize, mode, flags, fd, 0);
	if (node_root == MAP_FAILED)perror("nodes.bin MAP_FAILED");
	close(fd);

	fd = open_map("statements.bin", statementsSize);

	statement_root = (Statement *) mmap(((char *) node_root) + nodesSize, statementsSize, mode, flags, fd, 0);
	if (statement_root == MAP_FAILED)perror("statements.bin MAP_FAILED");
	close(fd);


	fd = open_map("contexts.bin", contextSize);
	context_root = contexts = (Context *) mmap(statement_root + statementsSize, contextSize, mode, flags, fd, 0);
	if (contexts == MAP_FAILED)perror("contexts.bin MAP_FAILED ");
	close(fd);
	context = &contexts[0];
//	context->nodeNames = name_root;// hack:
//	fixNodeNames(context, (char *) 0x2e8000000);// hack
	if (context->nodeNames != name_root)
		fixNodeNames(context, context->nodeNames);
	if(context->statements != statement_root){
		printf("WRONG IMPORT");
		exit(0);
	}
	if(context->nodes != node_root+propertySlots){
		printf("WRONG IMPORT");
		exit(0);
	}
	if(context->lastNode > maxNodes-propertySlots)
	{
		printf("OUT OF MEMORY");
		printf("WRONG IMPORT");
		exit(0);
	}

//	initContext(context);// NO, IT SHOULD BE ALL SET!
//	collectAbstracts();// was empty!
}

void load(bool force) {
	if (USE_MMAP) {
		loadMemoryMaps();
		return;
	}
//	clock_t start=clock();
//	double diff= ( std::clock() - start ) / (double)CLOCKS_PER_SEC;

	Context *c = getContext(current_context);
	Node *oldNodes = c->nodes;
	size_t read = 0;
	char *oldnodeNames = c->nodeNames;
	oldnodeNames = initContext(c);// allocate 64GB shared memory

	if (!force and context_root) {
		ps("loaded from shared memory");
//		if (virgin_memory or !hasWord("instance"))
//			collectAbstracts(); //or load abstracts->bin
		showContext(wordnet);
		return;
	}

	ps("Loading graph from files!");

	FILE *fp = open_binary("contexts.bin");
	if (fp == NULL) {
		p("contexts.bin missing! starting with fresh empty context!");
		clearMemory();
		return;
		//    exit(1);
	} else {
		read = fread(contexts, sizeof(Context), maxContexts, fp);
		printf("read %zu entries\n", read);
		fclose(fp);
	}
	fp = open_binary("names.bin");
	read = fread(name_root, sizeof(char), c->currentNameSlot + 100, fp);
	printf("read %zu entries\n", read);
	fclose(fp);

	fp = open_binary("statements.bin");
	read = fread(c->statements, sizeof(Statement), maxStatements, fp); // c->statementCount
	printf("read %zu entries\n", read);
	fclose(fp);

	fp = open_binary("nodes.bin");
	read = fread(c->nodes - propertySlots, sizeof(Node), maxNodes, fp);//c->nodeCount
	printf("read %zu entries\n", read);
	fclose(fp);

	if (oldNodes != c->nodes) {
		p("Fixing nodes");
		fixStatementNodeIds(c, oldNodes);
	}


	fp = open_binary("abstracts.bin");
	if (fp) {
		read = fread(abstracts, sizeof(Ahash), maxNodes * 2, fp);
		printf("read %zu entries\n", read);
		fclose(fp);
	} else {
		ps("collecting abstracts!");
		collectAbstracts(); //or load abstracts->bin
	}
	initRelations();
	showContext(context);

	// cout<<"nanoseconds "<< diff <<'\n';
}


void fixPointers() {
	if (context->nodeNames == name_root)
		return;// all INT now, no more pointers!!! except chars
	p("ADJUSTING SHARED MEMORY DISABLED");
	return;
//	if(!checkC)
	//	showContext(context);
	fixPointers(context);
	//	showContext(context);
	//	context = context;// &contexts[wordnet]; // todo: all
	//	showContext(context);
	//	fixPointers(context);
	showContext(context);
	collectAbstracts();
}

void fixPointers(Context *context) {
	p("ADJUSTING Context");
	Node *oldNodes = context->nodes;
	char *oldNames = context->nodeNames;
	initContext(context);
//	check((char*) context->nodes == context_root + contextOffset);
	//	context->nodes=root_memory
	fixStatementNodeIds(context, oldNodes);
	fixNodeNames(context, oldNames);
}

int collectAbstracts(bool clear/*=false*/) {
	ps("collecting abstracts = buildAbstractHashes");// int now
	initRelations();
	if (clear) {
		p("WIPING OLD abstracts!");
		memset(abstracts, 0, maxNodes * sizeof(Ahash) * 2);
	}
	Context *c = context;
//	int max=c->nodeCount; // maxNodes;
	int count = 0;
	// collect Abstracts
	for (int i = 0; i < max(c->nodeCount, context->lastNode); i++) {
		Node *n = &c->nodes[i];
		if (i > 1000 and !checkNode(n, i, 0, 1, 0)) continue;// checkStatements=0, bool checkNames,bool report
		if (n == null or n->name == null or n->id == 0) continue;
		if (n->kind == Abstract->id or (!clear and !hasWord(n->name))) {
			insertAbstractHash(n);// don't force
			count++;
		}
	}
	return count;
}

int collectInstances() {
	ps("collecting instances");// int now
	initRelations();
	memset(abstracts, 0, maxNodes * sizeof(Ahash) * 2);
	Context *c = context;
	int max = c->nodeCount; // maxNodes;
	int count = 0;
	// collect Abstracts
	for (int i = 0; i < max; i++) {
		Node *n = &c->nodes[i];
		if (i > 1000 and !checkNode(n)) break;
		if (n == null or n->name == null or n->id == 0) continue;
		if (n->kind == Abstract->id) {
			insertAbstractHash(n);
			count++;
		} else {
			addStatement(getAbstract(n->name), Instance, n, false);
		}
	}
	return count;
}

void fixNodeNames(Context *context, char *oldnodeNames) {
#ifdef inlineName
	printf("inlineNames!");
	return;
#endif
	if (LIVE) {
		printf("LIVE mode: Can NOT change Node Names!");
		return;
	}
	printf("fix Node Names!");
	// char* as offset to context->nodeNames would fix that too, but make names MUCH harder to debug
	long newOffset = context->nodeNames - oldnodeNames;
	if (newOffset == 0)return;
	int max = context->nodeCount; // maxNodes;
	for (int i = 0; i < max; i++) {
		Node *n = &context->nodes[i];
		if (!checkNode(n, i, 0, 0)) continue;
//		if (n->name < (char *) 0 or n->name > context->nodeNames + maxChars)
//			n->name = 0;// f'd up
		if (n->name) {
			n->name = n->name + newOffset;
//			while (n->name < context->nodeNames)
//				n->name += newOffset;// hack
		}
		n->name = n->name;
		//		show(n,true);
	}
}

bool clearMemory() {
//	releaseSharedMemory();
	if (!virgin_memory) {
		ps("Cleansing Memory!");
		if (USE_MMAP) {
			p("memset context 0");
			if (maxNodes <= 10 * million) {
				p("memset FULL WIPE, this takes a minute or 10");
				memset(node_root, 0, sizeof(Node) * maxNodes); //calloc!
				memset(statement_root, 0, sizeof(Statement) * maxStatements);
				memset(name_root, 0, sizeof(char) * maxChars);
				memset(abstracts, 0, sizeof(Ahash) * maxNodes * 2);// expensive, but no other way
			} else {
//			dd < /dev/zero bs=16777216 count=1 > contexts.bin LIMITED TO 2GB
				memset(node_root, 0, sizeof(Node) * context->nodeCount + propertySlots + 1000); //calloc!
				memset(statement_root, 0, sizeof(Statement) * context->statementCount + 1000);
				memset(name_root, 0, sizeof(char) * context->currentNameSlot + 1000);
				memset(abstracts, 0, sizeof(Ahash) * maxNodes * 2);// expensive, but no other way
			}
			p("memset context 0 done");
		} else {
			detach_shared_memory();
			initSharedMemory();
		}
		virgin_memory = false;
	}
	initRootContext();
	initRelations();
	return true;
}

char *initContext(Context *context) {
//	pf("Initiating context %d\n", context->id);
	Node *nodes = 0;
	Statement *statements = 0;
	char *nodeNames = 0;
//	int nodeSize=sizeof(Node); // 40
//	int statementSize=sizeof(Statement); //
//	int ahashSize=sizeof(Ahash); //
	//  int contextOffset=sizeof (Context) *maxContexts;
	long nameSegmentSize = sizeof(char) * averageNameLength * maxNodes;
	long nodeSegmentSize = sizeof(Node) * maxNodes;
	long statementSegmentSize = sizeof(Statement) * maxStatements;
//	long abstractOffset=contextOffset + nodeSegmentSize + nameSegmentSize + statementSegmentSize; //just put them at the end!!
	if (node_root) {
//		p("Multiple shared memory segments");
		nodes = (Node *) node_root + propertySlots;
		nodeNames = (char *) name_root;
		statements = (Statement *) statement_root;
	} else if (context_root) {
		p("ONE shared memory segment");
		if (contextOffset + nodeSegmentSize + nameSegmentSize + statementSegmentSize >
		    sizeOfSharedMemory + sizeof(Ahash) * 2) { //
			ps("ERROR sizeOfSharedMemory TOO SMALL!");
			ps("contextOffset+nodeSegmentSize+nameSegmentSize+statementSegmentSizeabstractSegment > sizeOfSharedMemory !");
			p(contextOffset + nodeSegmentSize + nameSegmentSize + statementSegmentSize);
			p(sizeOfSharedMemory);
			exit(1);
		}
		nodes = (Node *) (context_root + contextOffset);
		nodeNames = (char *) &context_root[contextOffset + nodeSegmentSize];
		statements = (Statement *) &context_root[contextOffset + nodeSegmentSize + nameSegmentSize];
	} else
		do {
			p("NO shared memory -> MALLOC memory segments");
			// 'POTENTIAL LEAK' OK! keep alive as long as app runs!
#ifndef __clang_analyzer__
			statements = (Statement *) malloc(statementSegmentSize + 1);
			nodes = (Node *) malloc(nodeSegmentSize + 1);
			nodeNames = (char *) malloc(nameSegmentSize + 1);
#endif
			//    nodeNames=(char*)malloc(sizeof(char)*nameBatch);// incremental
			if (nodes == 0 or statements == 0 or nodeNames == 0) {
				ps("System has not enough memory to support ");
				printf("%ld Nodes and %ld Statements\nDividing by 2 ...\n", maxNodes, maxStatements);
				maxStatements = maxStatements / 2;
				maxNodes = maxNodes / 2;
				// negotiate memory cleverly!!
			}
		} while (nodes == 0 or statements == 0);
//	if (!context_root or virgin_memory) clearMemory();
//	Statement* oldstatements=context->statements;
	if (!context)context = getContext(current_context, false);
	char *oldnodeNames = context->nodeNames;
//	Node* oldnodes=context->nodes;
	context->nodes = nodes;
	context->statements = statements;
	context->nodeNames = nodeNames;
	if (context->name[0] == 0)strcpy(context->name, "Public");
	if (context->currentNameSlot <= 0)
		context->currentNameSlot = 1;
	if (context->statementCount == 0)// ??
		context->statementCount = 1;// 0 = error
	if (context->nodeCount <= 0)
		context->nodeCount = 1;
	if (context->lastNode <= 0)
		context->lastNode = 1;

//	px(context);
//	px(nodes);
//	px(nodeNames);
//	px(statements);
//	px(abstracts);
//	px(extrahash);
	//		check((Node*)nodeNames==&context->nodes[maxNodes]);// statements == bounds of nodes
	return oldnodeNames;
}

#ifdef sharedLib
/* The shared library's read-only segment (in particular, its .text section) can be shared among all processes;
 * while its write-able sections (such as .data and .bss) can be allocated uniquely for each executing process.
 * This write-able segment is also referred to as the object's Static Data Segment.
 * It is this static data segment that creates most of the complexity for the implementation of shared libraries.
 http://www.cadenux.com/xflat/NoMMUSharedLibs.html#shlibs */
//#define sharedLib
void __attribute__((constructor)) my_init(void) {
	if (debug)
	printf("loading shared library \n");
	share_memory();
	if (context_root[0] != 1)
	buildDictionary(); //4sec.
	else if (debug)
	printf("attached to shared memory of library libDictionary.so \n");
	// saveDictionary();
	// loadDictionary();
	return 0;
}
#endif
extern "C" void allowWipe() {
	_allowWipe = true;
}


extern char *getConfig(const char *name) {
	if (file_exists("netbase.config")) {
		FILE *infile = open_file("netbase.config");
		char line[1000];
//		char* line=(char*)malloc(1000);
//		char* value=(char*)malloc(1000);
		while (fgets(line, sizeof(line), infile) != NULL) {
			if (line[0] == '#')continue;
			fixNewline(line);
			char *key = line;
			char *value = index(line, '=');
			if (!value)continue;
			value[0] = 0;
			value++;
//			int ok=sscanf(line, "%[^=]s=%s", key, value);// fuck c!
// todo ignore space / camelcase data_path == dataPath
			if (eq(key, name, true, true)) {
				printf("got netbase.config setting %s=%s\n",key,value);
				return clone(value);
			}
		}
//		pf("PROPERTY MISSING IN netbase.config : %s\n",name)
	}
	return getenv(name);
}

bool isTrue(char *c) {
	return eq(c, "true") || eq(c, "1") || eq(c, "yes");
}

long long maxChars = 10 * million;// SET VIA netbase.config !
long maxNodes = 10 * million;// *40byte => 400MB  300*million;// Live 11.11.2018

// => 400MB  300*million;// Live 11.11.2018
long maxStatements = 2 * maxNodes;
long sizeOfSharedMemory = 0; // overwrite here:

void loadConfig() {// char* args
	p("load netbase.config environment variables or fall back to defaults");
	if (getConfig("PORT"))SERVER_PORT = atoi(getConfig("PORT"));
	if (getConfig("SERVER_PORT"))SERVER_PORT = atoi(getConfig("SERVER_PORT"));

	if (getConfig("limit")){
		lookupLimit = atoi(getConfig("limit"));
		resultLimit = atoi(getConfig("limit"));
	}
	if (getConfig("resultLimit"))resultLimit = atoi(getConfig("resultLimit"));
	if (getConfig("lookupLimit"))lookupLimit = atoi(getConfig("lookupLimit"));
	if (getConfig("german"))useGermanLabels = isTrue(getConfig("german"));
	if (getConfig("germanLabels"))useGermanLabels = isTrue(getConfig("germanLabels"));
	if (getConfig("LIVE"))LIVE = isTrue(getConfig("LIVE"));
	if (getConfig("USE_MMAP"))USE_MMAP = isTrue(getConfig("USE_MMAP"));
	if (getConfig("data_path"))data_path = getConfig("data_path");
	if (getConfig("dataPath"))data_path = getConfig("dataPath");
	if (getConfig("maxNodes")) {
		maxNodes = atol(getConfig("maxNodes"));
		if (maxNodes < 10000)maxNodes = maxNodes * million;
		if (getConfig("maxStatements"))
			maxStatements = atol(getConfig("maxStatements"));
		else
			maxStatements = maxNodes * 2;
		if (maxStatements < 10000)maxStatements = maxStatements * million;
		if (getConfig("maxChars"))
			maxChars = atol(getConfig("maxChars"));
		else
			maxChars = maxNodes * averageNameLength;
		if (maxChars < 10000)maxChars = maxChars * million;
	}
	sizeOfSharedMemory = contextOffset + maxNodes * bytesPerNode + maxStatements * sizeof(Statement);
}

