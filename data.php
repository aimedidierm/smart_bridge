<?php
require 'php-includes/connect.php';
if(isset($_POST['car'])){
    $sql ="INSERT INTO reported (leftside) VALUES (1)";
    $stm = $db->prepare($sql);
    $stm->execute();
}
?>