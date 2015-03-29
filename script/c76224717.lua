--アドバンス・ゾーン
function c76224717.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--destroy/draw/salvage
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(76224717,0))
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_PHASE+PHASE_END)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1)
	e2:SetTarget(c76224717.target)
	e2:SetOperation(c76224717.operation)
	c:RegisterEffect(e2)
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
function c76224717.filter1(c)
	return c:IsFacedown() and c:IsDestructable()
end
function c76224717.filter2(c)
	return c:IsType(TYPE_MONSTER) and c:IsAbleToHand()
end
function c76224717.target(e,tp,eg,ep,ev,re,r,rp,chk)
	local b1=c76224717[tp]>0 and Duel.IsExistingMatchingCard(c76224717.filter1,tp,0,LOCATION_ONFIELD,1,nil)
	local b2=c76224717[tp]>1 and Duel.IsPlayerCanDraw(tp,1)
	local b3=c76224717[tp]>2 and Duel.IsExistingMatchingCard(c76224717.filter2,tp,LOCATION_GRAVE,0,1,nil)
	if chk==0 then return b1 or b2 or b3 end
	local cat=0
	if b1 then
		cat=cat+CATEGORY_DESTROY
		local g=Duel.GetMatchingGroup(c76224717.filter1,tp,0,LOCATION_ONFIELD,nil)
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
	end
	if b2 then
		cat=cat+CATEGORY_DRAW
		Duel.SetOperationInfo(0,CATEGORY_DRAW,nil,0,tp,1)
	end
	if b3 then
		cat=cat+CATEGORY_TOHAND
		Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_GRAVE)
	end
	e:SetCategory(cat)
end
function c76224717.operation(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local act=false
	if c76224717[tp]>0 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
		local g=Duel.SelectMatchingCard(tp,c76224717.filter1,tp,0,LOCATION_ONFIELD,1,1,nil)
		if g:GetCount()>0 then
			Duel.Destroy(g,REASON_EFFECT)
			act=true
		end
	end
	if c76224717[tp]>1 and Duel.IsPlayerCanDraw(tp,1) then
		if act then Duel.BreakEffect() end
		Duel.Draw(tp,1,REASON_EFFECT)
		act=true
	end
	if c76224717[tp]>2 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local g=Duel.SelectMatchingCard(tp,c76224717.filter2,tp,LOCATION_GRAVE,0,1,1,nil)
		if g:GetCount()>0 then
			if act then Duel.BreakEffect() end
			Duel.SendtoHand(g,nil,REASON_EFFECT)
			Duel.ConfirmCards(1-tp,g)
		end
	end
end
