<?php

/**
 * @file
 * A basic migration example.
 */

/**
 * Comments here are limited to CiviMigrate extension - more information is available in
 * the migrate_example module
 *
 * This is a very simple example drawing from a single table
 */
class peopleMigration extends Civimigration {
  protected $base_table = 'civimigrate_example_people_names';
  protected $base_table_id = 'aid'; // name of id field
  protected $base_table_alias = 'names';
  protected $entity = 'contact'; // this is the default
  protected $debug = 0; // set to 1 for debug info
  public function __construct() {
    parent::__construct();
    $this->description = t('Import people table');

    /*
     * If you have no defined the base table above & the base_table_id the parent class will define the mapping otherwise
     * see migrate_example module for how
     *
     * Destination is defined by parent based on the $entity defined above
     */

    /*
     * Define the query
     * getQuery is a civimigrate shortcut for adding all the fields in the table with the table
     * name as a prefix - ie name is added as names_name. The id is not prefixes as this causes errors
     * the prefixing is convenient if you add more than one table with similaly named columns.
     * Alternatively you can use any db_select syntax to generate the query- check the beer migration for an example.
     */
    $query = $this->getQuery($this->base_table,$this->base_table_alias,$this->base_table_id)
              ->condition('status', 1); // only include those with status = 1
    $this->source = new MigrateSourceSQL($query);

    /*
     * Add the field mappings
     */
    $this->addFieldMapping('first_name', 'names_name');
    $this->addFieldMapping('contact_type')->defaultValue('Individual');
    $this->addFieldMapping(null, 'names_posted')->description('To discuss');
  }
}



