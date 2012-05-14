<?php 
abstract class Civimigration_drupal6 extends Civimigration{
  
  protected $drupal = 6;
  public function __construct() {
    parent::__construct();
  }

  /*
   * Get specific field in table
   */  
  protected function gettableField($table, $column){

    $fields = array();
    $sql = "SHOW columns from {$this->base_table} WHERE field = '%s'";
    $result = db_query($sql,$this->base_table_id);
    while ($record = db_fetch_object($result)) {
      $record->null = $record->Null;
      $record->field = $record->Field;
      $record->type = $record->Type;
      return   $record;
    }
  }
  
  /*
   * Get all fields in table
   */  
  protected function gettableFields($table, $alias = ""){

    $fields = array();
    $sql = "SHOW COLUMNS FROM {$table}";
    $result = db_query($sql);
    while ($record = db_fetch_object($result)) {
      $fields[] =   $record->Field;
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
    while ($record = db_fetch_object($result)) {
      $query->addField($alias, $record->Field, $alias . "_" . strtolower($record->Field));
    }
  }
  
}
