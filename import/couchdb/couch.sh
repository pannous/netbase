curl http://127.0.0.1:5984/example/_design/csv?revs=true
curl -vX DELETE http://localhost:5984/example/_design/csv?rev=2-6cb2d2bcb0784f7d69e8f1b67dc600a0
curl -vX DELETE http://localhost:5984/example/_design/csv?rev=undefined
curl -vX PUT http://localhost:5984/example/_design/csv -d @couch.view.json 
curl http://127.0.0.1:5984/example/_design/csv/_view/all
curl http://127.0.0.1:5984/example/_design/csv/_list/cvs/all