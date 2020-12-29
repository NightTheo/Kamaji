INSERT INTO EQUIPMENT(id,name,category) VALUES (1,'Ecran','éléctronique');
INSERT INTO EQUIPMENT(id,name,category) VALUES (2,'Camera','éléctronique');
INSERT INTO EQUIPMENT(id,name,category) VALUES (3,'WhiteBoard','support écriture');
INSERT INTO EQUIPMENT(id,name,category) VALUES (4,'Video-projecteur','éléctronique');

INSERT INTO PLACE(id,name,address,map) VALUES(1,'ESGI','242 rue faubourg saint aintoi','plan/planESGI.jpg');
INSERT INTO PLACE(id,name,address,map) VALUES(2,'ICAN','21 Rue Erard','plan/planICAN.jpg');
INSERT INTO PLACE(id,name,address,map) VALUES(3,'PPA','28-32 Rue de l\'Amiral Mouchez','plan/planPPA.jpg');

INSERT INTO ROOM(id,name,max_capacity,price_half_day,place) VALUES (1,'A12',6,19,1);
INSERT INTO ROOM(id,name,max_capacity,price_half_day,place) VALUES (2,'A22',10,30,1);
INSERT INTO ROOM(id,name,max_capacity,price_half_day,place) VALUES (3,'Salle 2',4,15,2);
INSERT INTO ROOM(id,name,max_capacity,price_half_day,place) VALUES (4,'Salle 3',8,14,3);


INSERT INTO BOOKING(id,nb_persons,price,date_booking,time_slot,state,room) VALUES(1,6,25,'2020-11-24','14h - 20h',1,1);
INSERT INTO BOOKING(id,nb_persons,price,date_booking,time_slot,state,room) VALUES(2,8,34,'2020-11-24','8h - 14h',1,1);
INSERT INTO BOOKING(id,nb_persons,price,date_booking,time_slot,state,room) VALUES(3,8,40,'2020-11-24','14h - 20h',1,2);

INSERT INTO DRINK(id,name,category) VALUES(1,'café','boisson chaude');
INSERT INTO DRINK(id,name,category) VALUES(2,'thé','boisson froide');

INSERT INTO _place_propose_drink(place,drink,price) VALUES(1,1,2);
INSERT INTO _place_propose_drink(place,drink,price) VALUES(1,2,1);

INSERT INTO _booking_include_drink(booking,drink)VALUES(1,1);

INSERT INTO _room_owns_equipment(room,equipment,price)VALUES(1,1,6);
INSERT INTO _room_owns_equipment(room,equipment,price)VALUES(1,2,8);
