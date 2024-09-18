#include "Database.h"
#include "Logger.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
    // Initialize the database
    Database db("mydatabase.txt");

    // Set index type
    db.setIndexType("BTREE");

    // User registration and login
    db.executeQuery("REGISTER alice password123");
    db.executeQuery("REGISTER bob secretpass");
    db.executeQuery("LOGIN alice password123");

    // Create a table
    db.executeQuery("CREATE users id INT name VARCHAR(100)");

    // Insert records
    db.executeQuery("INSERT Alice 25");
    db.executeQuery("INSERT Bob 30");

    // Query records
    db.executeQuery("QUERY 1"); // Assuming IDs are numeric and 1 is valid

    // Update records (Assuming an update operation exists in the Database class, you need to implement it)
    // db.executeQuery("UPDATE users SET name = 'Charlie' WHERE id = 1");

    // Select records
    db.executeQuery("SELECT");

    // Begin a transaction
    db.executeQuery("BEGIN");

    // Insert records during transaction
    db.executeQuery("INSERT Charlie 28");

    // Commit the transaction
    db.executeQuery("COMMIT");

    // Remove a record
    db.executeQuery("REMOVE 2"); // Assuming ID 2 exists

    // Rollback a transaction (To demonstrate rollback, you would need to perform operations within a transaction and roll back)
    db.executeQuery("BEGIN");
    db.executeQuery("INSERT Dave 35");
    db.executeQuery("ROLLBACK");

    // Backup the database
    db.executeQuery("BACKUP mydatabase_backup.txt");

    // Restore the database
    db.executeQuery("RESTORE mydatabase_backup.txt");

    // Logout the user
    db.executeQuery("LOGOUT");

    // Drop a table
    db.executeQuery("DROP users");

    return 0;
}
