<?php

 /**
 * Destination class implementing migration into users.
 */
class MigrateDestinationCivicrmApi extends MigrateDestinationEntity {
  static $entity;
  static $debug = 0;
  static public function getKeySchema() {


   return array(
      'id' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
        'description' => 'ID of API destination',
      ),
    );

  }

  /**
   * Return an options array for user destinations.
   *
   * @param string $language
   *  Default language for usrs created via this destination class.
   * @param string $text_format
   *  Default text format for users created via this destination class.
   */
 /* static public function options($language, $text_format) {
    return compact('language', 'text_format');
  }
  */

  /**
   * Basic initialization
   *
   * @param array $options
   *  Options applied to comments.
   */
  public function __construct($entity,array $options = array()) {
    $this->entity = $entity;
    $this->debug = FALSE;
    parent::__construct('civicrm_' .$this->entity, 'CiviCRM', $options);
  }

  /**
   * Returns a list of fields available to be mapped for users
   *
   * @return array
   *  Keys: machine names of the fields (to be passed to addFieldMapping)
   *  Values: Human-friendly descriptions of the fields.
   */
  public function fields() {
  if(!civicrm_initialize()){
    return;
  }
  require_once 'CRM/Utils/Array.php';
	$fields = civicrm_api($this->entity,'getfields',array('version' => 3, 'action' => 'create'));
  $values = array();
  foreach ($fields['values'] as $field => $valueArray) {
    $values[$field] = CRM_Utils_Array::value('title',$valueArray,CRM_Utils_Array::value('label',$valueArray,CRM_Utils_Array::value('name',$valueArray)));  }
	  return $values;
  }

  /**
   * Delete a batch of users at once.
   *
   * @param $uids
   *  Array of user IDs to be deleted.
   */
  public function bulkRollback(array $ids) {
    if(!civicrm_initialize()){
      return;
    }
    migrate_instrument_start('civicrm_contact_delete_multiple');
    $this->prepareRollback($ids);
    foreach ($ids as $id) {
      try{
        civicrm_api($this->entity, 'delete', array('version' => 3, 'id' => $id, 'skip_undelete' => 1));
      } catch (Exception $e){
        throw new MigrateException('failed to delete ' . $id);
      }
    }
    $this->completeRollback($ids);
    migrate_instrument_stop('civicrm_contact_delete_multiple');
    CRM_Core_DAO::freeResult ();
  }

  /**
   * Import a single user.
   *
   * @param $account
   *  Account object to build. Prefilled with any fields mapped in the Migration.
   * @param $row
   *  Raw source data object - passed through to prepare/complete handlers.
   * @return array
   *  Array of key fields (uid only in this case) of the user that was saved if
   *  successful. FALSE on failure.
   */
  public function import(stdClass $paramsobj, stdClass $row) {
   if(!civicrm_initialize()){
      return;
   }
   if($this->debug ==1){
       drupal_set_message("<pre>".print_r($paramsobj,true)."</pre>");
       drupal_set_message("<pre>".print_r($row,true)."</pre>");
   }

    $migration = Migration::currentMigration();
    // Updating previously-migrated content?
    if (isset($row->migrate_map_destid1)) {
      if (isset($paramsobj->id)) {
        if ($params->id != $row->migrate_map_destid1) {
          throw new MigrateException(t("Incoming id !id and map destination id !destid1 don't match",
            array('!id' => $paramsobj->id, '!destid1' => $row->migrate_map_destid1)));
        }
      }
      else {
        $paramsobj->id = $row->migrate_map_destid1;
      }
    }
    if ($migration->getSystemOfRecord() == Migration::DESTINATION) {
      if (!isset($paramsobj->id)) {
        throw new MigrateException(t('System-of-record is DESTINATION, but no destination uid provided'));
      }
    //TODO - do anything with this - maybe not
    //  $old_account = user_load($params['id'], TRUE);
    }
    else {
    //TODO - do anything with this - maybe not
      // $old_account = $account;
    }

    $this->prepare($paramsobj, $row);
    if(isset($paramsobj->civimigrate_skip) && $paramsobj->civimigrate_skip == 1){
      throw new MigrateException('record intentionally skipped per job set up');
    }

    $paramsobj->version = 3;
    $params = (array) $paramsobj;
    if (isset($params['id']) ) {
      $updating = TRUE;
    }
    else {
      $updating = FALSE;
    }

    if($this->debug ==1){
       drupal_set_message("<pre>Params as passed to API".print_r($params,true)."</pre>");
    }
    migrate_instrument_start('civicrm_' . $this->entity . '_import');
    $apiresult = civicrm_api($this->entity,'create',$params);
    if($this->debug ==1){
       drupal_set_message("<pre>Result from API".print_r($apiresult,true)."</pre>");
    }
    migrate_instrument_stop('civicrm_' . $this->entity . '_import');
    if($apiresult['is_error'] ==1){
      throw new MigrateException($apiresult['error_message']);
    }
    if ($apiresult) {
      if ($updating) {
        $this->numUpdated++;
      }
      else {
        $this->numCreated++;
      }
      $this->complete($apiresult, $row);
      $return = array($apiresult['id']);
    }
    else {
      $return = FALSE;
    }
    return $return;
  }
}
