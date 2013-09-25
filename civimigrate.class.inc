<?php
abstract class Civimigration extends Migration {

  protected $base_table;
  protected $base_table_id;
  protected $base_table_alias;
  protected $query;
  protected $_db;
  protected $entity = 'contact';
  protected $debug = 0;
  protected $autoquery= 1;
  protected $matchexternalidentifier = 1;// if set then for contacts the external_identifier will be used to look up existing contacts
  public function __construct() {

    parent::__construct(MigrateGroup::getInstance('CiviCRM'));
    $this->destination = new MigrateDestinationCivicrmApi($this->entity);
    if($this->debug ==1){
      $this->destination->debug=1;
    }
    if(!empty($this->base_table) && !empty($this->base_table_id)){
      $column = $this->gettableField($this->base_table, $this->base_table_id);
      // 'type' is returned as 'int(11)' - need to separated if there is a length
      if(isset($column->type)){
        $type = explode('(', $column->type);
      }
      else{
        $type = explode('(', $column->Type);
      }
      $length = array();
      if (array_key_exists(1, $type)) { //length key
        $length = array(
          'length' => substr($type[1], 0, - 1)
        );
      }
      if(isset($column->null)){
        $null = $column->null;
      }
      else{
        $null = $column->Null;
      }
      $this->map = new MigrateSQLMap($this->machineName, array(
        $this->base_table_id => array(
          'type' => $type[0],
          'not null' => $null,
          'alias' => empty($this->base_table_alias) ? $this->base_table : $this->base_table_alias
        ) + $length
      ), MigrateDestinationCivicrmApi::getKeySchema());

      if($this->autoquery == 1){
        $query = $this->getQuery($this->base_table,$this->base_table_alias,$this->base_table_id);
        $this->source = new MigrateSourceSQL($query);
      }
    }
    $mappings = $this->getmappings();
    foreach ($mappings as $mapping) {
      $destField = empty($mapping->mappings_destination_field) ? null : $mapping->mappings_destination_field;
      $sourceField = empty($mapping->mappings_source_field) ? null : $mapping->mappings_source_field;
      if(empty($sourceField)){
        $this->addFieldMapping($destField)
        ->description($mapping->mappings_description)
        ->issueGroup($mapping->mappings_issuegroup)
        ->defaultValue($mapping->mappings_default) ;
      }else{
        $this->addFieldMapping($destField, $sourceField)
        ->description($mapping->mappings_description)
        ->issueGroup($mapping->mappings_issuegroup)
        ->defaultValue($mapping->mappings_default) ;
      }
    }
  }
  /*
   * You can override this but the default function does a lookup on external identifier
   * and does a match on that if present
   */
  public function prepare(&$entity, &$row) {
    if($this->matchexternalidentifier && strtolower($this->entity) == 'contact' && empty($entity->id)  && !empty($entity->external_identifier) ){
      $existingID = (civicrm_api('contact','getvalue',array('version' => 3,'return' => 'id', 'external_identifier' => $entity->external_identifier )));
      if(!empty($existingID) && !is_array($existingID)){
        $entity->id = $existingID;
      }
    }
  }
  /*
   * Get started DB object based on table
   */
  protected function getQuery($table,$alias, $idKey =""){

    $query = db_select( $table ,$alias);
    $this->addFieldsFromtable($query,$table,$alias, $idKey);
    return $query;
  }
/*
 * Add all fields to function (there is a function to do this as a * but
 * by adding them by name they become visible in the source fields in migrateUI
 */
  protected function addFieldsFromtable(&$query, $table, $alias, $idKey = null){
    $fields = $this->gettableFields($table, $alias);
    foreach ($fields as $field) {
      if(strtolower($field) == strtolower($idKey)){
        $query->addField($alias, $field, strtolower($field));
      }else{
        $query->addField($alias, $field, strtolower($alias . "_" . $field));
      }
    }
    return $query;
  }

  /*
   * Get specific field in table
   */
  protected function gettableField($table, $column){

   $columns = db_query("SHOW columns from {$this->base_table} WHERE field = :column",array( 'column' => $column ));
      foreach ($columns as $column ) {
        return $column;
      }
  }

 /*
   * Get specific field in table
   */
  protected function getmappings(){

    $mappingquery = $this->getQuery('civimigrate_mappings','mappings');
    $mappingquery->condition('migration', $this->machineName);
    $mappings = $mappingquery->execute();
    return $mappings;
  }

  /*
   * Get all fields in table
   */
  protected function gettableFields($table, $alias = ""){

    $fields = array();
    $sql = "SHOW COLUMNS FROM {$table}";
    $result = db_query($sql);
    foreach ($result as $record) {
      $fields[] =   $record->field;
    }
    return $fields;
  }

  /*
   * Get all fields in table
   */
  protected function addtableFields(&$query,$table, $alias = ""){
    if(empty($alias)){
      $alias = $table;
    }
    $fields = array();
    $sql = "SHOW COLUMNS FROM {$table}";
    $result = db_query($sql);
    foreach ($result as $record) {
      $query->addField($alias, $record->field, $alias . "_" . strtolower($record->field));
    }
  }

  /*
   * Convert Microsoft Timestamp to mysql friendly date.
   * @param string $ms_date MS date string
   * @param Bool $force Definitely needs conversion - by default function tests if it is already a valid timestamp
   * (this is in case data is inconsistently being sent through)
   *
   */
   function mssql2mysql($ms_date, $force = 0)
   {
     if(empty($ms_date)){
       return null;
     }
     if(!strtotime($ms_date) || $force)
     $timestamp = ($ms_date - 25569) * 86400;
     return date('Y-m-d',$timestamp);
   }


 }
