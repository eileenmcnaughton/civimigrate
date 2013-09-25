<?php

class CiviMigrateFileItemJSON extends MigrateItemJSON {

  protected $data = array();
  protected $keyField = NULL;

  /**
   * Here we are expectingto be loading from file not url so we need to cope with that
   * Taken from https://drupal.org/node/1152160 for starting point
   * @author eileen
   *
   */

  public function __construct($item_url, $httpOptions, $keyField){
    $this->keyField = $keyField;
    parent::__construct($item_url, $httpOptions, $item_url);
  }
  /**
   * Two problems with the default getItem() - it reads and parses the JSON on
   * each call, which we don't want to do for multiple items in the file; and,
   * we need to retrieve a given item from the file according to its 'id' element.
   */
  public function getItem($id) {
    // We cache the parsed JSON at $this->data.
    if (empty($this->data)) {
      $data = $this->loadJSONUrl($this->itemUrl);
      if ($data) {
        // Let's index the array by the ID for easy retrieval.
        foreach ($data as $item) {
          $this->data[$item->{$this->keyField}] = $item;
        }
      }
      else {
        // Error-handling here....
      }
    }
    // Return the requested item
    if (isset($this->data[$id])) {
      return $this->data[$id];
    }
    else {
      return NULL;
    }
  }
}

/**
 * Get ids from same file as items
 * Here we are expectingto be loading from file not url so we need to cope with that
 * Taken from https://drupal.org/node/1152160 for starting point
 * @author eileen
 *
 */
class CiviMigrateListJSON extends MigrateListJSON {
  private $keyField;

  function __construct($item_url, $keyField) {
    $this->keyField = $keyField;
    parent::__construct($item_url);
  }
  /**
   * The default implementation assumes the IDs are top-level array elements,
   * but the array elements are the data items - we need to look inside them
   * for the IDs.
   */
  protected function getIDsFromJSON(array $data) {
    $ids = array();
    foreach ($data as $item) {
      $ids[] = $item[$this->keyField];
    }
    return $ids;
  }
}
