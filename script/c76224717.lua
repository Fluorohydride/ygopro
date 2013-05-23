--アドバンス・ゾーン
function c76224717.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--search
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(76224717,0))
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCode(EVENT_PHASE+PHASE_END)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1)
	e2:SetCondition(c76224717.condition)
	e2:SetCost(c76224717.cost)
	e2:SetTarget(c76224717.target1)
	e2:SetOperation(c76224717.operation1)
	e2:SetLabel(1)
	c:RegisterEffect(e2)
	--draw
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(76224717,1))
	e3:SetCategory(CATEGORY_DRAW)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e3:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e3:SetCode(EVENT_PHASE+PHASE_END)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCountLimit(1)
	e3:SetCondition(c76224717.condition)
	e3:SetCost(c76224717.cost)
	e3:SetTarget(c76224717.target2)
	e3:SetOperation(c76224717.operation2)
	e3:SetLabel(2)
	c:RegisterEffect(e3)
	--salvage
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(76224717,2))
	e4:SetCategory(CATEGORY_TOHAND)
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e4:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e4:SetCode(EVENT_PHASE+PHASE_END)
	e4:SetRange(LOCATION_SZONE)
	e4:SetCountLimit(1)
	e4:SetCondition(c76224717.condition)
	e4:SetCost(c76224717.cost)
	e4:SetTarget(c76224717.target3)
	e4:SetOperation(c76224717.operation3)
	e4:SetLabel(3)
	c:RegisterEffect(e4)
	if not c76224717.global_check then
		c76224717.global_check=true
		c76224717[0]=0
		c76224717[1]=0
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_SUMMON_SUCCESS)
		ge1:SetOperation(c76224717.checkop)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_MSET)
		ge2:SetOperation(c76224717.checkop)
		Duel.RegisterEffect(ge2,0)
		local ge3=Effect.CreateEffect(c)
		ge3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge3:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		ge3:SetOperation(c76224717.clear)
		Duel.RegisterEffect(ge3,0)
	end
end
function c76224717.checkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	if bit.band(tc:GetSummonType(),SUMMON_TYPE_ADVANCE)==SUMMON_TYPE_ADVANCE then
		c76224717[ep]=c76224717[ep]+tc:GetMaterialCount()
	end
end
function c76224717.clear(e,tp,eg,ep,ev,re,r,rp)
	c76224717[0]=0
	c76224717[1]=0
end
function c76224717.condition(e,tp,eg,ep,ev,re,r,rp)
	return c76224717[tp]>=e:GetLabel()
end
function c76224717.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.Hint(HINT_OPSELECTED,1-tp,e:GetDescription())
end
function c76224717.filter1(c)
	return c:IsFacedown() and c:IsDestructable()
end
function c76224717.target1(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkc:IsControler(1-tp) and c76224717.filter1(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c76224717.filter1,tp,0,LOCATION_ONFIELD,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c76224717.filter1,tp,0,LOCATION_ONFIELD,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c76224717.operation1(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFacedown() then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
function c76224717.target2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsPlayerCanDraw(tp,1) end
	Duel.SetTargetPlayer(tp)
	Duel.SetTargetParam(1)
	Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
end
function c76224717.operation2(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Draw(p,d,REASON_EFFECT)
end
function c76224717.filter3(c)
	return c:IsType(TYPE_MONSTER) and c:IsAbleToHand()
end
function c76224717.target3(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c76224717.filter3(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c76224717.filter3,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectTarget(tp,c76224717.filter3,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c76224717.operation3(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,tc)
	end
end
