from flask import Flask, make_response, request, render_template, send_file, Response, redirect, url_for, abort, session
from flask.views import MethodView
from flask_login import login_user, logout_user, UserMixin, LoginManager, login_required

from datetime import datetime, timedelta
import os
import re
import stat
import mimetypes

galaxy_viewer_app = Flask("galaxy_viewer", static_url_path='/assets', static_folder='assets')
galaxy_viewer_app.config.update(
    SECRET_KEY="galaxy_viewer"
)
root = os.path.normpath("/tmp")
key = ""

ignored = ['.bzr', '$RECYCLE.BIN', '.DAV', '.DS_Store', '.git', '.hg', '.htaccess', '.htpasswd', '.Spotlight-V100',
           '.svn', '__MACOSX', 'ehthumbs.db', 'robots.txt', 'Thumbs.db', 'thumbs.tps']
datatypes = {
    'audio': 'm4a,mp3,oga,ogg,webma,wav',
    'archive': '7z,zip,rar,gz,tar',
    'image': 'gif,ico,jpe,jpeg,jpg,png,svg,webp',
    'pdf': 'pdf',
    'quicktime': '3g2,3gp,3gp2,3gpp,mov,qt',
    'source': 'atom,bat,bash,c,cc,h,cmd,coffee,css,hml,js,json,yaml,java,less,markdown,md,php,pl,py,rb,rss,sass,scpt,'
              'swift,scss,sh,xml,yml,plist,ipynb,BUILD,WORKSPACE',
    'text': 'txt,pbtxt',
    'video': 'mp4,m4v,ogv,webm',
    'website': 'htm,html,mhtm,mhtml,xhtm,xhtml',
}
icontypes = {
    'fa-music': 'm4a,mp3,oga,ogg,webma,wav',
    'fa-archive': '7z,zip,rar,gz,tar',
    'fa-picture-o': 'gif,ico,jpe,jpeg,jpg,png,svg,webp',
    'fa-file-text': 'pdf',
    'fa-film': '3g2,3gp,3gp2,3gpp,mov,qt,mp4,m4v,ogv,webm',
    'fa-code': 'atom,plist,bat,bash,c,cc,h,cmd,coffee,css,hml,js,json,yaml,java,less,markdown,md,php,pl,py,rb,rss,sass,'
               'scpt,swift,scss,sh,xml,yml,BUILD,WORKSPACE',
    'fa-file-text-o': 'txt,pbtxt',
    'fa-globe': 'htm,html,mhtm,mhtml,xhtm,xhtml',
}

login_manager = LoginManager()
login_manager.init_app(galaxy_viewer_app)
login_manager.login_view = "login"


class User(UserMixin):

    def get_id(self):
        return "galaxy_viewer".encode('utf-8')


@galaxy_viewer_app.before_request
def make_session_permanent():
    session.permanent = True
    galaxy_viewer_app.permanent_session_lifetime = timedelta(minutes=30)


@login_manager.user_loader
def load_user(user_id):
    try:
        return User()
    except Exception as err:
        # pslx_frontend_logger.error("Load user with error message: " + str(err) + '.')
        return None


@galaxy_viewer_app.route('/login', methods=['POST', 'GET'])
def login():
    if request.method == 'POST':
        # galaxy_viewer_app.info('Index logging in with ' + str(dict(request.form)) + " from ip [" +
        #                           request.remote_addr + '].')
        if request.form['username'] == os.getenv('GALAXY_fs_viewer_username') and \
                request.form['password'] == os.getenv('GALAXY_fs_viewer_password'):
            user = User()
            login_user(user)
            return redirect(request.args.get('next'))
        else:
            abort(401)
    else:
        return render_template('login.html')


@galaxy_viewer_app.route('/logout')
@login_required
def logout():
    logout_user()
    return redirect(url_for('index'))


@galaxy_viewer_app.template_filter('size_fmt')
def size_fmt(size):
    return size


@galaxy_viewer_app.template_filter('time_fmt')
def time_desc(timestamp):
    mdate = datetime.fromtimestamp(timestamp)
    str = mdate.strftime('%Y-%m-%d %H:%M:%S')
    return str


@galaxy_viewer_app.template_filter('data_fmt')
def data_fmt(filename):
    t = 'unknown'
    for type, exts in datatypes.items():
        if filename.split('.')[-1] in exts.split(','):
            t = type
    return t


@galaxy_viewer_app.template_filter('icon_fmt')
def icon_fmt(filename):
    i = 'fa-file-o'
    for icon, exts in icontypes.items():
        if filename.split('.')[-1] in exts.split(','):
            i = icon
    return i


@galaxy_viewer_app.template_filter('humanize')
def time_humanize(timestamp):
    mdate = datetime.utcfromtimestamp(timestamp)
    return mdate


def get_type(mode):
    if stat.S_ISDIR(mode) or stat.S_ISLNK(mode):
        type = 'dir'
    else:
        type = 'file'
    return type


def partial_response(path, start, end=None):
    file_size = os.path.getsize(path)

    if end is None:
        end = file_size - start - 1
    end = min(end, file_size - 1)
    length = end - start + 1

    with open(path, 'rb') as fd:
        fd.seek(start)
        bytes = fd.read(length)
    assert len(bytes) == length

    response = Response(
        bytes,
        206,
        mimetype=mimetypes.guess_type(path)[0],
        direct_passthrough=True,
    )
    response.headers.add(
        'Content-Range', 'bytes {0}-{1}/{2}'.format(
            start, end, file_size,
        ),
    )
    response.headers.add(
        'Accept-Ranges', 'bytes'
    )
    return response


def get_range(request):
    range = request.headers.get('Range')
    m = re.match('bytes=(?P<start>\d+)-(?P<end>\d+)?', range)
    if m:
        start = m.group('start')
        end = m.group('end')
        start = int(start)
        if end is not None:
            end = int(end)
        return start, end
    else:
        return 0, None


class PathView(MethodView):
    @login_required
    def get(self, p=''):
        path = os.path.join(root, p)
        if os.path.isdir(path):
            contents = []
            total = {'size': 0, 'dir': 0, 'file': 0}
            for filename in os.listdir(path):
                if filename in ignored or filename[0] == '.':
                    continue

                filepath = os.path.join(path, filename)
                stat_res = os.stat(filepath)
                info = {}
                info['name'] = filename
                info['mtime'] = stat_res.st_mtime
                ft = get_type(stat_res.st_mode)
                info['type'] = ft
                total[ft] += 1
                sz = stat_res.st_size
                info['size'] = sz
                total['size'] += sz
                contents.append(info)
            page = render_template('index.html', path=p, contents=contents, total=total, hide_dotfile="yes")
            res = make_response(page, 200)
        elif os.path.isfile(path):
            if 'Range' in request.headers:
                start, end = get_range(request)
                res = partial_response(path, start, end)
            else:
                res = send_file(path)
                res.headers.add('Content-Disposition', 'attachment')
        else:
            res = make_response('Not found', 404)
        return res

    @login_required
    def post(self, p=''):
        path = request.form.get('search_path').rstrip('/')
        return redirect(url_for('path_view', p=path + '/'))


path_view = PathView.as_view('path_view')
galaxy_viewer_app.add_url_rule('/', view_func=path_view)
galaxy_viewer_app.add_url_rule('/index.html', view_func=path_view)
galaxy_viewer_app.add_url_rule('/<path:p>', view_func=path_view)
galaxy_viewer_app.add_url_rule('/search', view_func=path_view)

if __name__ == '__main__':
    bind = os.getenv('FS_BIND', '0.0.0.0')
    port = os.getenv('FS_PORT', '8000')
    root = os.path.normpath(os.getenv('FS_PATH', '/home/pslx'))
    key = os.getenv('FS_KEY', "mafia2018")
    galaxy_viewer_app.run(bind, port, threaded=True, debug=True)
