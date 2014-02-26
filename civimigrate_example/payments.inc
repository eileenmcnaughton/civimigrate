<?php

/**
 * @file
 * A basic migration example.
 */

/**
 * Comments here are limited to CiviMigrate extension - more information is available in
 * the migrate_example module
 */
class paymentsMigration extends Civimigration {
  protected $base_table = 'civimigrate_example_people_payments';
  protected $base_table_id = 'payment_id';
  protected $base_table_alias = 'payments';
  protected $entity = 'contribution'; // this is the default
  protected $debug = 0;
  public function __construct($arguments = array()) {
    parent::__construct($arguments);
    $this->description = t('Import people table');


    //getQuery is a civimigrate shortcut for adding all the fields in the table with the table
    //name as a prefix - ie
    $query = $this->getQuery($this->base_table,$this->base_table_alias,$this->base_table_id);
    $this->source = new MigrateSourceSQL($query);
    $this->dependencies = array('people');
    /*
     * The line below says 'take the value of the person_id field and look up what it was mapped to
     * during the previous peopleMigration import and use that for the contact_id
     */
    $this->addFieldMapping('contact_id','payments_person_id')
                 ->sourceMigration('people');
    $this->addFieldMapping('total_amount', 'payments_amount');
    $this->addFieldMapping('receive_date','payments_date');
    $this->addFieldMapping('source','payments_description');
    //this last mapping we will adjust in the prepare function - an alternative is to make the SQL statement more intelligent - e.g. join to a mappings table
    $this->addFieldMapping('contribution_type_id')->defaultValue(1);
  }

    public function prepare(&$entity, &$row) {
      // for a simple tweak this is nice & easy!
      if($row->payments_code == 'Fee'){
        $entity->contribution_type_id = 2;
      }
    }
}


