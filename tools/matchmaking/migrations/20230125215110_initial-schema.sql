CREATE TABLE users (
    device_id     integer,
    licence_id    smallint,

    mii           bytea   NOT NULL,
    race_rating   integer NOT NULL,
    friend_suffix text    NOT NULL, -- Random, used at the end of the friend code
    location      integer NOT NULL,
    latitude      integer NOT NULL,
    longitude     integer NOT NULL,

    PRIMARY KEY (device_id, licence_id)
);

CREATE TABLE friendships (
    friender_device_id  integer,
    friender_licence_id smallint,

    friendee_device_id  integer,
    friendee_licence_id smallint,

    confirmed bool NOT NULL,

    PRIMARY KEY (friender_device_id, friender_licence_id, friendee_device_id, friendee_licence_id),
    FOREIGN KEY (friender_device_id, friender_licence_id) REFERENCES users (device_id, licence_id) ON DELETE CASCADE,
    FOREIGN KEY (friendee_device_id, friendee_licence_id) REFERENCES users (device_id, licence_id) ON DELETE CASCADE
);
