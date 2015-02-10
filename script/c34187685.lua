--ダブルアタック
function c34187685.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetLabel(0)
	e1:SetCondition(c34187685.condition)
	e1:SetCost(c34187685.cost)
	e1:SetTarget(c34187685.target)
	e1:SetOperation(c34187685.activate)
	c:RegisterEffect(e1)
end
function c34187685.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnCount()~=1 and Duel.GetCurrentPhase()==PHASE_MAIN1
		and not Duel.IsPlayerAffectedByEffect(tp,EFFECT_CANNOT_BP)
end
function c34187685.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(1)
	return true
end
function c34187685.filter1(c,tp)
	local lv=c:GetOriginalLevel()
	return lv>1 and c:IsDiscardable() and c:IsAbleToGraveAsCost()
		and Duel.IsExistingTarget(c34187685.filter2,tp,LOCATION_MZONE,0,1,nil,lv)
end
function c34187685.filter2(c,lv)
	return c:IsFaceup() and c:IsLevelBelow(lv-1) and not c:IsHasEffect(EFFECT_EXTRA_ATTACK)
end
function c34187685.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c34187685.filter2(chkc,e:GetLabel()) end
	if chk==0 then
		if e:GetLabel()~=1 then return false end
		e:SetLabel(0)
		return Duel.IsExistingMatchingCard(c34187685.filter1,tp,LOCATION_HAND,0,1,nil,tp)
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DISCARD)
	local cg=Duel.SelectMatchingCard(tp,c34187685.filter1,tp,LOCATION_HAND,0,1,1,nil,tp)
	Duel.SendtoGrave(cg,REASON_DISCARD+REASON_COST)
	local lv=cg:GetFirst():GetLevel()
	e:SetLabel(lv)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c34187685.filter2,tp,LOCATION_MZONE,0,1,1,nil,lv)
end
function c34187685.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_EXTRA_ATTACK)
		e1:SetValue(1)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e1)
	end
end
