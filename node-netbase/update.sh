cd ..
make
cd -
if [ $APPLE ] 
then
	cp ../netbase lib/netbase.dylib 
else
	cp ../netbase lib/netbase.so
fi

./increase-package-version.js
npm publish ./ 

git commit -a --allow-empty-message -m ''
git push --all 
git status 