--ゲーテの魔導書
function c97997309.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(97997309,0))
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetCondition(c97997309.condition)
	e1:SetCost(c97997309.cost)
	e1:SetTarget(c97997309.target1)
	e1:SetOperation(c97997309.activate1)
	e1:SetLabel(1)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(97997309,1))
	e2:SetCategory(CATEGORY_POSITION)
	e2:SetType(EFFECT_TYPE_ACTIVATE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetCode(EVENT_FREE_CHAIN)
	e2:SetHintTiming(0,0x1c0+TIMING_BATTLE_PHASE)
	e2:SetCondition(c97997309.condition)
	e2:SetCost(c97997309.cost)
	e2:SetTarget(c97997309.target2)
	e2:SetOperation(c97997309.activate2)
	e2:SetLabel(2)
	c:RegisterEffect(e2)
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(97997309,2))
	e3:SetCategory(CATEGORY_REMOVE)
	e3:SetType(EFFECT_TYPE_ACTIVATE)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetCode(EVENT_FREE_CHAIN)
	e3:SetHintTiming(0,0x1e0)
	e3:SetCondition(c97997309.condition)
	e3:SetCost(c97997309.cost)
	e3:SetTarget(c97997309.target3)
	e3:SetOperation(c97997309.activate3)
	e3:SetLabel(3)
	c:RegisterEffect(e3)
end
function c97997309.cfilter(c)
	return c:IsFaceup() and c:IsRace(RACE_SPELLCASTER)
end
function c97997309.rfilter(c)
	return c:IsSetCard(0x106e) and c:IsType(TYPE_SPELL) and c:IsAbleToRemoveAsCost()
end
function c97997309.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c97997309.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c97997309.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	local ct=e:GetLabel()
	if chk==0 then return Duel.GetFlagEffect(tp,97997309)==0
		and Duel.IsExistingMatchingCard(c97997309.rfilter,tp,LOCATION_GRAVE,0,ct,nil) end
	Duel.Hint(HINT_OPSELECTED,1-tp,e:GetDescription())
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectMatchingCard(tp,c97997309.rfilter,tp,LOCATION_GRAVE,0,ct,ct,nil)
	Duel.Remove(g,POS_FACEUP,REASON_EFFECT)
	Duel.RegisterFlagEffect(tp,97997309,RESET_PHASE+PHASE_END,EFFECT_FLAG_OATH,1)
end
function c97997309.filter1(c)
	return c:IsFacedown() and c:IsAbleToHand()
end
function c97997309.target1(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_SZONE) and c97997309.filter1(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c97997309.filter1,tp,LOCATION_SZONE,LOCATION_SZONE,1,e:GetHandler()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
	local g=Duel.SelectTarget(tp,c97997309.filter1,tp,LOCATION_SZONE,LOCATION_SZONE,1,1,e:GetHandler())
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,g:GetCount(),0,0)
end
function c97997309.activate1(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFacedown() and tc:IsRelateToEffect(e) then
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
	end
end
function c97997309.filter2(c)
	return not c:IsPosition(POS_FACEUP_ATTACK) or c:IsCanTurnSet()
end
function c97997309.target2(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c97997309.filter2(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c97997309.filter2,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_POSITION)
	local g=Duel.SelectTarget(tp,c97997309.filter2,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,g,g:GetCount(),0,0)
end
function c97997309.activate2(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		if tc:IsPosition(POS_FACEUP_ATTACK) then
			Duel.ChangePosition(tc,POS_FACEDOWN_DEFENCE)
		else
			local pos=Duel.SelectPosition(tp,tc,POS_FACEUP_ATTACK+POS_FACEDOWN_DEFENCE)
			Duel.ChangePosition(tc,pos)
		end
	end
end
function c97997309.filter3(c)
	return c:IsAbleToRemove()
end
function c97997309.target3(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsOnField() and chkcLIsControler(1-tp) and c97997309.filter3(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c97997309.filter3,tp,0,LOCATION_ONFIELD,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectTarget(tp,c97997309.filter3,tp,0,LOCATION_ONFIELD,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,g:GetCount(),0,0)
end
function c97997309.activate3(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.Remove(tc,POS_FACEUP,REASON_EFFECT)
	end
end
