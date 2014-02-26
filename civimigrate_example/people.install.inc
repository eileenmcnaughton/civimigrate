<?php

/**
 * @file
 * Set up for the beer (basic) example.
 */
function civimigrate_example_people_schema() {
  $schema['civimigrate_example_people_names'] = civimigrate_example_people_schema_names();
  $schema['civimigrate_example_people_payments'] = civimigrate_example_people_schema_payments();
  return $schema;
}

function civimigrate_example_people_install() {
  // Populate our tables.
  civimigrate_example_people_data_names();
  civimigrate_example_people_data_payments();

}

function civimigrate_example_people_uninstall() {
}

function civimigrate_example_people_disable() {
  Migration::deregisterMigration('BeerTerm');
  Migration::deregisterMigration('BeerUser');
}



function civimigrate_example_people_schema_payments() {
  return array(
    'description' => 'Payments Received',
    'fields' => array(
      'payment_id'  => array(
        'type' => 'serial',
        'not null' => TRUE,
        'description' => 'Amount',
      ),
      'person_id'  => array(
        'type' => 'int',
        'not null' => TRUE,
        'description' => 'Amount',
      ),
      'description'  => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
      ),
      'amount'  => array(
        'type' => 'int',
        'not null' => TRUE,
        'description' => 'Amount',
      ),
      'code' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => FALSE,
        'description' => 'Donation or Fee',
      ),
      'date' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => FALSE,
        'description' => 'date received',
      ),
    ),
    'primary key' => array('payment_id'),
  );
}

function civimigrate_example_people_schema_names() {
  return array(
    'description' => 'Beers accounts.',
    'fields' => array(
      'aid'  => array(
        'type' => 'serial',
        //'not null' => TRUE,
        'description' => 'Account ID',
      ),
      'status'  => array(
        'type' => 'int',
        'not null' => TRUE,
        'description' => 'Blocked_Allowed',
      ),
      'posted' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
        'description' => 'Registration date',
      ),
      'name' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => FALSE,
        'description' => 'Account name (for login)',
      ),
      'nickname' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => FALSE,
        'description' => 'Account name (for display)',
      ),
      'password' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => FALSE,
        'description' => 'Account password (raw)',
      ),
      'mail' => array(
        'type' => 'varchar',
        'length' => 255,
        'not null' => FALSE,
        'description' => 'Account email',
      ),
      'sex' => array(
        'type' => 'int',
        'not null' => FALSE,
        'description' => 'Gender',
      ),
    ),
    'primary key' => array('aid'),
  );
}


function civimigrate_example_people_data_names() {
  $fields = array('status', 'posted', 'name', 'nickname', 'password', 'mail', 'sex');
  $query = db_insert('civimigrate_example_people_names')
    ->fields($fields);
  $data = array(
    array(1, '2010-03-30 10:31:05', 'alice', 'alice hot pants', 'alicepass', 'alice@example.com', '1'),
    array(1, '2010-04-04 10:31:05', 'alice', 'alice dupe pants', 'alicepass', 'alice2@example.com', '1'),
    array(0, '2007-03-15 10:31:05', 'bob', 'rebob', 'bobpass', 'bob@example.com', '1'),
    array(1, '2004-02-29 10:31:05', 'charlie', 'charlie chocolate', 'mykids', 'charlie@example.com', '0'),
  );
  foreach ($data as $row) {
    $query->values(array_combine($fields, $row));
  }
  $query->execute();
}

function civimigrate_example_people_data_payments() {
  $fields = array('person_id', 'date', 'description', 'code', 'amount');
  $query = db_insert('civimigrate_example_people_payments')
    ->fields($fields);
  $data = array(
    array(1, '2010-03-30 10:31:05', 'alice\'s first payment', 'Donation', '50'),
    array(1, '2010-04-04 10:31:05', 'alice\'s second payment', 'Fee', '60'),
    array(2, '2007-03-15 10:31:05', 'bob\'s first payment', 'Fee', '20'),
    array(3, '2004-02-29 10:31:05', 'charlie\'s first payment', 'Donation', '10'),
  );
  foreach ($data as $row) {
    $query->values(array_combine($fields, $row));
  }
  $query->execute();
}
