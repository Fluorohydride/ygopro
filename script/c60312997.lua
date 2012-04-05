--波動再生
function c60312997.initial_effect(c)
	local e1=Effect.CreateEffect(c)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetCondition(c60312997.condition)
	e1:SetTarget(c60312997.target)
	e1:SetOperation(c60312997.operation)
	c:RegisterEffect(e1)
end
function c60312997.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp and Duel.GetAttackTarget()==nil
		and eg:GetFirst():IsLocation(LOCATION_MZONE)
end
function c60312997.filter(c,lv)
	return c:IsLevelBelow(lv) and c:IsType(TYPE_SYNCHRO) and c:IsStatus(STATUS_PROC_COMPLETE)
end
function c60312997.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c60312997.filter(chkc,eg:GetFirst():GetLevel()) end 
	if chk==0 then return Duel.IsExistingTarget(c60312997.filter,tp,LOCATION_GRAVE,0,1,nil,eg:GetFirst():GetLevel()) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	Duel.SelectTarget(tp,c60312997.filter,tp,LOCATION_GRAVE,0,1,1,nil,eg:GetFirst():GetLevel())
end
function c60312997.operation(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CHANGE_DAMAGE)
	e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e1:SetReset(RESET_PHASE+PHASE_DAMAGE)
	e1:SetTargetRange(1,0)
	e1:SetValue(c60312997.damval)
	Duel.RegisterEffect(e1,tp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		local e2=Effect.CreateEffect(e:GetHandler())
		e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e2:SetCode(EVENT_DAMAGE_STEP_END)
		e2:SetOperation(c60312997.spop)
		e2:SetLabelObject(tc)
		e2:SetReset(RESET_PHASE+PHASE_DAMAGE)
		Duel.RegisterEffect(e2,tp)
	end
end
function c60312997.damval(e,re,val,r,rp,rc)
	if r==REASON_BATTLE then
		return val/2
	else return val end
end
function c60312997.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local tc=e:GetLabelObject()
	if tc:IsLocation(LOCATION_GRAVE) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
