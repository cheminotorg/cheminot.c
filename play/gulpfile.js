var gulp = require('gulp');
var gutil = require('gulp-util');
var watchify = require('watchify');
var browserify = require('browserify');
var source = require('vinyl-source-stream');

gulp.task('default', function() {
  var bundleStream = browserify('./src/main.js', {debug: true}).bundle();

  return bundleStream
    .on('error', function(error) { gutil.log(gutil.colors.red(error.message)); })
    .pipe(source('app.js'))
    .pipe(gulp.dest('./www'));
});

gulp.task('watch', function() {
  var opts = watchify.args;
  opts.debug = true;

  var bundleStream = watchify(browserify('./src/main.js', opts));

  function rebundle() {
    return bundleStream.bundle()
      .on('error', function(error) { gutil.log(gutil.colors.red(error.message)); })
      .pipe(source('app.js'))
      .pipe(gulp.dest('./www'));
  }

  bundleStream.on('update', rebundle);
  bundleStream.on('log', gutil.log);

  return rebundle();
});