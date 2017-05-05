<?php

$dir = "<insert_writable_directory_here>";

if(isset($_GET['username']) && isset($_GET['score'])) {

    $username = $_GET['username'];
    $score = $_GET['score'];

    $esc_usr = preg_replace("/[^a-zA-Z0-9]+/", "", $username);
    $esc_cmd =
        "DATE=$(date +'%Y-%m-%d_%H-%M-%S') &&".
        "mkdir -p '$dir/$esc_usr/' && ".
        "echo $score > $dir/$esc_usr/\$DATE || echo 'fail'";

    $update_result = shell_exec($esc_cmd);

    if($update_result == 'fail') {
        echo "Failed saving score";
    }

    $rowQuery = ' awk -v u="$user" \'END { print u "\\t" sum "\\t" max } BEGIN { max = -1 } { if ($1>max) { max = $1 }; sum += $1 }\'';
    $rowQueryEsc = str_replace('$', '\$', str_replace('"', '\"', $rowQuery));
    $cdCmd = ' cd "'.$dir.'"';
    $userScoreQuery = ' cat ./'.$esc_usr.'/* | '.$rowQuery.' | sed \'s/^/'.$esc_usr.'\t/\'';
    $scoresQuery = $cdCmd.' && find .  -type d ! -name .  -exec bash -c "user=\$(basename {}); cat {}/* | '.$rowQueryEsc.' " \; | sort -k 2 -g -r | head -10 && '.$userScoreQuery.' || echo fail';
    $topScores = shell_exec($scoresQuery);

    if($topScores == 'fail') {
        echo "Error querying top scores";
    } else {
        echo $topScores;
    }

} else {

    echo "File not found";

}

?>
