/**
 * Display tweets tagged with "#weblabor".
 */
Drupal.twitterWidget = function(res) {

    // localized version of http://twitter.com/javascripts/blogger.js
    function relative_time(time_value) {
        var values = time_value.split(" ");
        // pl. Mon, 06 Jul 2009 13:33:36 +0000
        time_value = values[2] + " " + values[1] + ", " + values[3] + " " + values[4];
        var parsed_date = Date.parse(time_value);
        var relative_to = (arguments.length > 1) ? arguments[1] : new Date();
        var delta = parseInt((relative_to.getTime() - parsed_date) / 1000);
        delta = delta + (relative_to.getTimezoneOffset() * 60);

        if (delta < 60) {
            return 'alig egy perce';
        } else if(delta < 120) {
            return 'egy perce';
        } else if(delta < (60*60)) {
            return (parseInt(delta / 60)).toString() + ' perce';
        } else if(delta < (120*60)) {
            return 'egy órája';
        } else if(delta < (24*60*60)) {
            return 'mintegy ' + (parseInt(delta / 3600)).toString() + ' órája';
        } else if(delta < (48*60*60)) {
            return 'egy napja';
        } else {
            return (parseInt(delta / 86400)).toString() + ' napja';
        }
    }

    var i, len,
        html = [],
        twit, txt,
        twits = res.results;
    for (i = 0, len = twits.length; i < len; i++) {
        twit = twits[i];
        txt = twit.text;
        // url tagify
        txt = txt.replace(/(https?:\/\/[^ ]+)/g, '<a href="$1">$1</a>');
        txt = txt.replace(/@(\w+)/g, '<a href="http://twitter.com/$1">$1</a>');
        // link other hashtags
        txt = txt.replace(/([ ^])#(\w+)/g, '$1<a href="http://twitter.com/search?q=%23$2">#$2</a>');
        // skip empty twits
        if (txt.replace(/ /g, '') == '') {
            continue;
        }

        html.push('<p class="vcard hentry"><a title="' + twit.from_user + '" rel="external" class="url" href="http://twitter.com/' + twit.from_user + '"><img width="24" height="24" class="photo" src="' + twit.profile_image_url.replace(/_normal/, '_mini') + '" alt="' + twit.from_user + '" /></a> <cite><a title="' + twit.from_user + '" rel="external" class="twitter-user fn n author" href="http://twitter.com/' + twit.from_user + '">' + twit.from_user + '</a></cite> <q class="entry-title entry-content">' + txt + '</q> <a class="published" rel="external bookmark" href="http://twitter.com/' + twit.from_user + '/statuses/' + twit.id + '">' + relative_time(twit.created_at) + '</a></p>');
    }
    $('#block-wltwitter-list').html(html.join("\n"));

};
