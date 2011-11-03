#!/bin/sh

DB=${1:-zim}

date
echo "create primary key in DB $DB"
echo "alter table words add primary key (word, aid, pos);"|psql $DB

date
echo "create foreign key in DB $DB"
echo "alter table words add foreign key (aid) references article;"|psql $DB

date
echo "create words_ix1 index in DB $DB"
echo "create index words_ix1 on words(aid);"|psql $DB

date
echo ready
