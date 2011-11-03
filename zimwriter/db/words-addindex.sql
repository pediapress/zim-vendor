alter table words add primary key (word, aid, pos);
alter table words add foreign key (aid) references article;
create index words_ix1 on words(aid);
