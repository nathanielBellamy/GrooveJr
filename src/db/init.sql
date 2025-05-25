
create table if not exists tracks (
  id integer primary key autoincrement,
  file_path text not null,
  title text,
  sf_frames integer not null,
  sf_samplerate integer not null,
  sf_channels integer not null,
  created_at datetime default current_timestamp
);