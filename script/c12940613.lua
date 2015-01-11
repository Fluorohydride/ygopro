--マドルチェ・マナー
function c12940613.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_ATKCHANGE+CATEGORY_TODECK)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_DAMAGE_STEP)
	e1:SetCondition(c12940613.condition)
	e1:SetTarget(c12940613.target)
	e1:SetOperation(c12940613.activate)
	c:RegisterEffect(e1)
end
function c12940613.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()~=PHASE_DAMAGE or not Duel.IsDamageCalculated()
end
function c12940613.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x71)
end
function c12940613.tdfilter1(c)
	return c:IsType(TYPE_MONSTER) and c:IsSetCard(0x71) and c:IsAbleToDeck()
end
function c12940613.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_GRAVE) and c12940613.tdfilter1(chkc) end
	if chk==0 then return Duel.IsExistingMatchingCard(c12940613.filter,tp,LOCATION_MZONE,0,1,nil)
		and Duel.IsExistingTarget(c12940613.tdfilter1,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
	local g=Duel.SelectTarget(tp,c12940613.tdfilter1,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_TODECK,g,1,0,0)
end
function c12940613.tdfilter2(c)
	return c:IsType(TYPE_MONSTER) and c:IsAbleToDeck()
end
function c12940613.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SendtoDeck(tc,nil,2,REASON_EFFECT)
	end
	local g=Duel.GetMatchingGroup(c12940613.filter,tp,LOCATION_MZONE,0,nil)
	tc=g:GetFirst()
	if not tc then return end
	while tc do
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(800)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		tc:RegisterEffect(e1)
		local e2=e1:Clone()
		e2:SetCode(EFFECT_UPDATE_DEFENCE)
		tc:RegisterEffect(e2)
		tc=g:GetNext()
	end
	local dg=Duel.GetMatchingGroup(c12940613.tdfilter2,tp,LOCATION_GRAVE,0,nil)
	if dg:GetCount()~=0 and Duel.SelectYesNo(tp,aux.Stringid(12940613,0)) then
		Duel.BreakEffect()
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TODECK)
		Duel.SendtoDeck(dg:Select(tp,1,1,nil),nil,2,REASON_EFFECT)
	end
end
