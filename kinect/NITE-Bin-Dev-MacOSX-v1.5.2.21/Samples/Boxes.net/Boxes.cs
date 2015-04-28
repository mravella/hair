using System;
using System.Collections.Generic;
using System.ComponentModel;
//using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace Boxes.net
{
    public partial class Boxes : Form
    {
        public Boxes()
        {
            InitializeComponent();

            this.context = OpenNI.Context.CreateFromXmlFile(SAMPLE_XML_FILE, out this.scriptNode);
            this.sessionManager = new NITE.SessionManager(this.context, "Wave", "RaiseHand");

            this.flowRouter = new NITE.FlowRouter();
            this.selectableSlider = new NITE.SelectableSlider1D(3, NITE.Axis.X);
            this.boxes = new MyBox[3];
            this.boxes[0] = new MyBox(this.Box1, "Box1");
            this.boxes[1] = new MyBox(this.Box2, "Box2");
            this.boxes[2] = new MyBox(this.Box3, "Box3");

            this.boxes[0].Leave += new MyBox.LeaveHandler(Boxes_Leave);
            this.boxes[1].Leave += new MyBox.LeaveHandler(Boxes_Leave);
            this.boxes[2].Leave += new MyBox.LeaveHandler(Boxes_Leave);

            this.selectableSlider.ItemHover += new EventHandler<NITE.IndexEventArgs>(selectableSlider_ItemHover);
            this.selectableSlider.ItemSelect += new EventHandler<NITE.IndexDirectionEventArgs>(selectableSlider_ItemSelect);
            this.selectableSlider.PrimaryPointCreate += new EventHandler<NITE.HandFocusEventArgs>(selectableSlider_PrimaryPointCreate);
            this.selectableSlider.PrimaryPointDestroy += new EventHandler<NITE.IdEventArgs>(selectableSlider_PrimaryPointDestroy);

            this.sessionManager.SessionStart += new EventHandler<NITE.PositionEventArgs>(sessionManager_SessionStart);

            this.sessionManager.AddListener(this.flowRouter);

            this.shouldRun = true;
            this.readerThread = new Thread(ReaderThread);
            this.readerThread.Start();
        }

        void selectableSlider_ItemSelect(object sender, NITE.IndexDirectionEventArgs e)
        {
            if (e.SelectionDirection == NITE.Direction.Up)
            {
                this.flowRouter.ActiveListener = this.boxes[e.Item];
            }
        }

        void sessionManager_SessionStart(object sender, NITE.PositionEventArgs e)
        {
            this.flowRouter.ActiveListener = this.selectableSlider;
        }

        void selectableSlider_PrimaryPointDestroy(object sender, NITE.IdEventArgs e)
        {
            this.SliderArea.BackColor = Color.White;
        }

        void selectableSlider_PrimaryPointCreate(object sender, NITE.HandFocusEventArgs e)
        {
            this.SliderArea.BackColor = Color.Red;
        }

        void selectableSlider_ItemHover(object sender, NITE.IndexEventArgs e)
        {
            switch (e.Item)
            {
                case 0:
                    this.Box1.BackColor = Color.Yellow;
                    this.Box2.BackColor = Color.LightBlue;
                    this.Box3.BackColor = Color.LightBlue;
                    break;
                case 1:
                    this.Box1.BackColor = Color.LightBlue;
                    this.Box2.BackColor = Color.Yellow;
                    this.Box3.BackColor = Color.LightBlue;
                    break;
                case 2:
                    this.Box1.BackColor = Color.LightBlue;
                    this.Box2.BackColor = Color.LightBlue;
                    this.Box3.BackColor = Color.Yellow;
                    break;
            }
        }

        void Boxes_Leave()
        {
            this.flowRouter.ActiveListener = this.selectableSlider;
        }

        private void Boxes_Load(object sender, EventArgs e)
        {
            // First time
            this.Box1.BackColor = Color.LightBlue;
            this.Box2.BackColor = Color.LightBlue;
            this.Box3.BackColor = Color.LightBlue;
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            this.flowRouter.ActiveListener = null;

            this.shouldRun = false;
            this.readerThread.Join();
            base.OnClosing(e);
        }
        protected override void OnKeyPress(KeyPressEventArgs e)
        {
            switch(e.KeyChar)
            {
                case (char)27:
                    Close();
                    break;
            }
            base.OnKeyPress(e);
        }

        private void ReaderThread()
        {
            while (this.shouldRun)
            {
                try
                {
                    this.context.WaitAnyUpdateAll();
                    this.sessionManager.Update(this.context);
                }
                catch (System.Exception)
                {
                	
                }
            }
        }

        private readonly string SAMPLE_XML_FILE = @"../../../Data/Sample-Tracking.xml";

        private OpenNI.Context context;
        private OpenNI.ScriptNode scriptNode;
        private Thread readerThread;
        private bool shouldRun;
        private NITE.SessionManager sessionManager;
        private NITE.FlowRouter flowRouter;
        private NITE.SelectableSlider1D selectableSlider;
        private MyBox[] boxes;

    }
}
