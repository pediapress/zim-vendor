create table mimetype
(
  id           serial primary key,
  mimetype     text not null,
  compress     boolean not null
);

insert into mimetype (mimetype, compress)
  select distinct mimetype, substring(mimetype from 1 for 5) <> 'image'
    from article
   where mimetype is not null;

alter table article
  add column mimetypeid integer;

update article
  set mimetypeid = (select id
                      from mimetype
                     where mimetype = article.mimetype)
 where mimetype is not null;

alter table article
  drop column mimetype;

alter table article
  rename column mimetypeid to mimetype;

alter table article
  add foreign key (mimetype) references mimetype;

alter table zimarticles
  rename to zimarticle;
