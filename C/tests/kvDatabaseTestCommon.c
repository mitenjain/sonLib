/*
 * Copyright (C) 2006-2011 by Benedict Paten (benedictpaten@gmail.com)
 *
 * Released under the MIT license, see LICENSE.txt
 */

/*
 * common code for database tests.
 */
#include "sonLibGlobalsTest.h"
#include "kvDatabaseTestCommon.h"
#include "stSafeC.h"
#include <getopt.h>

/* usage message and exit */
static void usage(const char *desc) {
    static const char *help = 
        "Options:\n"
        "\n"
        "-t --type=dbtype - one of 'TokyoTyrant', 'TokyoCabinet', 'MySql', or 'PostgreSql'.\n"
        "    Values area case-insensitive, defaults to TokyoCabinet.\n"
        "-d --db=database - database directory for TokyoCabinet or database name\n"
        "    for SQL databases. Defaults to testTCDatabase for TokyoCabinet,\n"
        "    SQL databases must specify.\n"
        "--host=host - SQL database host, defaults to localhost\n"
        "--port=port - SQL database port.\n"
        "-u, --user=user - SQL database user.\n"
        "-p, --pass=pass - SQL database password.\n"
        "-h, --help - print this message.\n";
    fprintf(stderr, "%s\n%s\n", desc, help);
    exit(1);
}

static stKVDatabaseType parseDbType(const char *dbTypeStr) {
    if (stString_eqcase(dbTypeStr, "TokyoCabinet")) {
        return stKVDatabaseTypeTokyoCabinet;
    } else if (stString_eqcase(dbTypeStr, "TokyoTyrant")) {
        return stKVDatabaseTypeTokyoTyrant;
    } else if (stString_eqcase(dbTypeStr, "MySql")) {
        return stKVDatabaseTypeMySql;
    } else if (stString_eqcase(dbTypeStr, "PostgreSql")) {
        return stKVDatabaseTypePostgreSql;
    } else {
        fprintf(stderr, "Error: invalid value for --type: %s\n", dbTypeStr);
        exit(1);
        return stKVDatabaseTypePostgreSql;
    }
}

/* Parse options for specifying database to tests.  Fill in positional argument vector, setting unused ones to NULL.
 * The positional and numPositionalRet maybe NULL. */
stKVDatabaseConf *kvDatabaseTestParseOptions(int argc, char *const *argv, const char *desc, int minNumPositional, int maxNumPositional,
                                             char **positional, int *numPositionalRet) {
    static struct option longOptions[] = {
        {"type", required_argument, NULL, 't'},
        {"db", required_argument,   NULL, 'd'},
        {"host", required_argument, NULL, 'H'},
        {"port", required_argument, NULL, 'P'},
        {"user", required_argument, NULL, 'u'},
        {"pass", required_argument, NULL, 'p'},
        {"help", no_argument,       NULL, 'h'},
        {NULL, 0, NULL, '\0'}
    };
    stKVDatabaseType optType = stKVDatabaseTypeTokyoCabinet;
    const char *optDb = "testTCDatabase";
    const char *optHost = "localhost";
    unsigned int optPort = 0;
    const char *optUser = NULL;
    const char *optPass = NULL;
    int optKey, optIndex;
    while ((optKey = getopt_long(argc, argv, "t:d:H:P:u:p:h", longOptions, &optIndex)) >= 0) {
        switch (optKey) {
        case 't':
            optType = parseDbType(optarg);
            break;
        case 'd':
            optDb = optarg;
            break;
        case 'H':
            optHost = optarg;
            break;
        case 'P':
            optPort = stSafeStrToUInt32(optarg);
            break;
        case 'u':
            optUser = optarg;
            break;
        case 'p':
            optPass = optarg;
            break;
        case 'h':
            usage(desc);
            break;
        default:
            fprintf(stderr, "Error: invalid  option: %s\n", argv[optind]); 
            usage(desc);
        }
    }
    int numPositional = argc - optind;
    if (!((minNumPositional <= numPositional) && (numPositional <= maxNumPositional))) {
        fprintf(stderr, "Error: wrong number of arguments\n");
        usage(desc);
    }
    if (positional != NULL) {
        int i;
        for (i = 0; i < numPositional; i++) {
            positional[i] = argv[optind+i];
        }
        for (; i < maxNumPositional; i++) {
            positional[i] = NULL;
        }
    }
    if (numPositionalRet != NULL) {
        *numPositionalRet = numPositional;
    }
    stKVDatabaseConf *conf = NULL;
    if (optType == stKVDatabaseTypeTokyoCabinet) {
        conf = stKVDatabaseConf_constructTokyoCabinet(optDb);
        fprintf(stderr, "running Tokyo Cabinet sonLibKVDatabase tests\n");
    } else if (optType == stKVDatabaseTypeMySql) {
        conf = stKVDatabaseConf_constructMySql(optHost, 0, optUser, optPass, optDb, "cactusDbTest");
        fprintf(stderr, "running MySQL sonLibKVDatabase tests\n");
    } else if (optType == stKVDatabaseTypePostgreSql) {
        conf = stKVDatabaseConf_constructPostgreSql(optHost, 0, optUser, optPass, optDb, "cactusDbTest");
        fprintf(stderr, "running PostgreSql sonLibKVDatabase tests\n");
    }
    return conf;
}

