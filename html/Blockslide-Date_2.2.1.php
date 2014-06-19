<!DOCTYPE html>
<html>
        <head>
                <title>BlockSlide-Date</title>
                <meta charset='utf-8'>
                <meta name='viewport' content='width=device-width, initial-scale=1'>
                <link rel='stylesheet' href='http://code.jquery.com/mobile/1.3.2/jquery.mobile-1.3.2.min.css' />
                <script src='http://code.jquery.com/jquery-1.9.1.min.js'></script>
                <script src='http://code.jquery.com/mobile/1.3.2/jquery.mobile-1.3.2.min.js'></script>
                <style>
                        .ui-header .ui-title { margin-left: 1em; margin-right: 1em; text-overflow: clip; }
                </style>
        </head>
		<body>
<div data-role="page" id="page1">
    <div data-theme="a" data-role="header" data-position="fixed">
        <h3>
            BlockSlide-Date Configuration
        </h3>
        <div class="ui-grid-a">
            <div class="ui-block-a">
                <input id="cancel" type="submit" data-theme="c" data-icon="delete" data-iconpos="left"
                value="Cancel" data-mini="true">
            </div>
            <div class="ui-block-b">
                <input id="save" type="submit" data-theme="b" data-icon="check" data-iconpos="right"
                value="Save" data-mini="true">
            </div>
        </div>
    </div>
    <div data-role="content">

<fieldset class="ui-grid-a">
  <div class="ui-block-a">
		<div id="dateorder" data-role="fieldcontain">
			<fieldset data-role="controlgroup" data-type="horizontal" data-mini="true">
				<legend>Date format</legend>

<?php
	if (!isset($_GET['dateorder'])) {
		$dateorder = 1;
	} else {
		$dateorder = $_GET['dateorder'];
	}
	
	if ($dateorder == 0) {
		$s1 = " checked";
		$s2 = "";
	} else {
		$s1 = "";
		$s2 = " checked";
	}
	
	echo '<input id="format1" name="dateorder" value="0" data-theme="" type="radio"' . $s1 . '><label for="format1">dd mm</label>';
	echo '<input id="format2" name="dateorder" value="1" data-theme="" type="radio"' . $s2 . '><label for="format2">mm dd</label>';
?>
			</fieldset>
		</div>
      <div id="stripes" data-role="fieldcontain">
        <fieldset data-role="controlgroup" data-type="vertical" data-mini="true">
          <legend>Stripes</legend>
<?php
	if (!isset($_GET['stripes'])) {
		$stripes = 1;
	} else {
		$stripes = $_GET['stripes'];
	}

	$s = array( "", "");
	$s[$stripes] = " checked";

	echo '<input id="stripes1" name="stripes" value="0" data-theme="" type="radio"' . $s[0] . '><label for="stripes1"><img src="digitsPlain.png" width="72" height="84"></label>';
	echo '<input id="stripes2" name="stripes" value="1" data-theme="" type="radio"' . $s[1] . '><label for="stripes2"><img src="digits1.png" width="72" height="84"></label>';
?>
        </fieldset>
      </div>

      <div id="roundcorners" data-role="fieldcontain">
        <fieldset data-role="controlgroup" data-type="vertical" data-mini="true">
          <legend>Corners</legend>
<?php
	if (!isset($_GET['roundcorners'])) {
		$roundcorners = 1;
	} else {
		$roundcorners = $_GET['roundcorners'];
	}

	$s = array( "", "");
	$s[$roundcorners] = " checked";

	echo '<input id="corners1" name="roundcorners" value="0" data-theme="" type="radio"' . $s[0] . '><label for="corners1"><img src="digitsNoCorners.png" width="72" height="84"></label>';
	echo '<input id="corners2" name="roundcorners" value="1" data-theme="" type="radio"' . $s[1] . '><label for="corners2"><img src="digits1.png" width="72" height="84"></label>';
?>
        </fieldset>
      </div>



  </div>
  <div class="ui-block-b">
        <div data-role="fieldcontain">
          <fieldset data-role="controlgroup" data-type="horizontal" data-mini="true">
            <legend>Show day of week</legend>
<?php
	if (!isset($_GET['weekday'])) {
		$weekday = 0;
	} else {
		$weekday = $_GET['weekday'];
	}
	
	if ($weekday == 0) {
		$s1 = " checked";
		$s2 = "";
	} else {
		$s1 = "";
		$s2 = " checked";
	}

	echo '<input id="weekday1" name="weekday" value="0" data-theme="" type="radio"' . $s1 . '><label for="weekday1">On</label>';
	echo '<input id="weekday2" name="weekday" value="1" data-theme="" type="radio"' . $s2 . '><label for="weekday2">Off</label>';
?>
        </fieldset>
        </div>
      <div id="fulldigits" data-role="fieldcontain">
        <fieldset data-role="controlgroup" data-type="vertical" data-mini="true">
          <legend>Digit Type</legend>
<?php
	if (!isset($_GET['fulldigits'])) {
		$fulldigits = 0;
	} else {
		$fulldigits = $_GET['fulldigits'];
	}

	$s = array( "", "");
	$s[$fulldigits] = " checked";

	echo '<input id="format1" name="fulldigits" value="0" data-theme="" type="radio"' . $s[0] . '><label for="format1"><img src="digits1.png" width="72" height="84"></label>';
	echo '<input id="format2" name="fulldigits" value="1" data-theme="" type="radio"' . $s[1] . '><label for="format2"><img src="digits2.png" width="72" height="84"></label>';
?>
        </fieldset>
      </div>
		<div data-role="fieldcontain">
			<label for="lang">
				Language
			</label>
			<select id="lang" data-native-menu="true" name="lang" data-mini="true">
<?php
	$langs = array(
		0 => 'Dutch',
		1 => 'English',
		2 => 'French',
		3 => 'German',
		5 => 'Italian',
		4 => 'Spanish'
	);
	
	if (!isset($_GET['lang'])) {
		$lang = 1;
	} else {
		$lang = $_GET['lang'];
	}
	
	foreach ($langs as $v => $n) {
		if ($lang == $v) {
			$s = " selected";
		} else {
			$s = "";
		}
		echo '<option value="' . $v . '" ' . $s . '>' . $n . '</option>';
	}
	?>
			</select>
		</div>
  </div>

	</div>
</div>

    <script>
      function saveOptions() {
        var options = {
          'dateorder': parseInt($("input[name=dateorder]:checked").val(), 10),
          'weekday': parseInt($("input[name=weekday]:checked").val(), 10),
          'lang': parseInt($("#lang").val(), 10),
          'stripes': parseInt($("input[name=stripes]:checked").val(), 10),
          'roundcorners': parseInt($("input[name=roundcorners]:checked").val(), 10),
          'fulldigits': parseInt($("input[name=fulldigits]:checked").val(), 10)
        }
        return options;
      }

      $().ready(function() {
        $("#cancel").click(function() {
          console.log("Cancel");
          document.location = "pebblejs://close#";
        });

        $("#save").click(function() {
          console.log("Submit");
          
          var location = "pebblejs://close#" + encodeURIComponent(JSON.stringify(saveOptions()));
          console.log("Close: " + location);
          console.log(location);
          document.location = location;
        });

      });
    </script>
</body>
</html>
