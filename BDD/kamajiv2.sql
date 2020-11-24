


/*
*   kamajiv1.sql
*
*
*   Project KAMAJI
*   ---- planning & booking rooms of coworking
*
*   created the 18 nov 2020
*__________________________
*   OMNES MOUHAMADOU Théo
*   TAGRI Yanis
*   2i2
*__________________________
*
*   database: kamajiv1
*
*/



create table PLACE(
  id INT AUTO_INCREMENT PRIMARY KEY,

  name VARCHAR(64),
  address VARCHAR(128),
  map VARCHAR(128),
  state BOOLEAN DEFAULT 1
);

create table ROOM(
  id INT AUTO_INCREMENT PRIMARY KEY,

  name VARCHAR(64),
  max_capacity INT,
  price_half_day INT,

  place INT UNIQUE NOT NULL,
  FOREIGN KEY (place) REFERENCES PLACE(id)
);

create table DRINK(
  id INT AUTO_INCREMENT PRIMARY KEY,

  name VARCHAR(64),
  category VARCHAR(64),
  state BOOLEAN DEFAULT 1
);

create table BOOKING(
  id INT AUTO_INCREMENT PRIMARY KEY,

  nb_persons INT,
  price INT,
  date_booking DATE,
  time_slot VARCHAR(10),
  state BOOLEAN DEFAULT 1,

  room INT NOT NULL,
  FOREIGN KEY (room) REFERENCES ROOM(id)
);

create table EQUIPMENT(
  id INT AUTO_INCREMENT PRIMARY KEY,

  name VARCHAR(64),
  category VARCHAR(64),
  state BOOLEAN DEFAULT 1
);


create table _place_propose_drink(
  place INT,
  drink INT,

  price INT,
  PRIMARY KEY(place,drink),
  FOREIGN KEY (place) REFERENCES PLACE(id),
  FOREIGN KEY (drink) REFERENCES DRINK(id)
);

create table _booking_include_drink(
  booking INT,
  drink INT,
  PRIMARY KEY(booking,drink),
  FOREIGN KEY (booking) REFERENCES BOOKING(id),
  FOREIGN KEY (drink) REFERENCES DRINK(id)
);

create table _room_owns_equipment(
  room INT,
  equipment INT,

  price INT,
  PRIMARY KEY(room,equipment),
  FOREIGN KEY (room) REFERENCES ROOM(id),
  FOREIGN KEY (equipment) REFERENCES EQUIPMENT(id)
);













/**/
