--シンクロ・ストライク
function c30123142.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_ATKCHANGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetHintTiming(TIMING_DAMAGE_STEP)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET+EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c30123142.condition)
	e1:SetTarget(c30123142.target)
	e1:SetOperation(c30123142.activate)
	c:RegisterEffect(e1)
end
function c30123142.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()~=PHASE_DAMAGE or not Duel.IsDamageCalculated()
end
function c30123142.filter(c)
	return c:IsFaceup() and c:GetSummonType()==SUMMON_TYPE_SYNCHRO and c:GetMaterialCount()~=0
end
function c30123142.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c30123142.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c30123142.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	Duel.SelectTarget(tp,c30123142.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil)
end
function c30123142.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and c30123142.filter(tc) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		e1:SetValue(tc:GetMaterialCount()*500)
		tc:RegisterEffect(e1)
	end
end
