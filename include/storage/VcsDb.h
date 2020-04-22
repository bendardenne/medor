//
// Created by bdardenn on 4/22/20.
//

#ifndef MEDOR_VCSDB_H
#define MEDOR_VCSDB_H

#include <sqlite3.h>

namespace medor::storage {
    class VcsDb {
    public:
        explicit VcsDb(sqlite3 *db_connection);


    private:
        sqlite3* _db;

        void setupDb();
    };


}

#endif //MEDOR_VCSDB_H
