--バトル・テレポーテーション
function c71652522.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c71652522.condition)
	e1:SetTarget(c71652522.target)
	e1:SetOperation(c71652522.activate)
	c:RegisterEffect(e1)
end
function c71652522.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetCurrentPhase()<=PHASE_BATTLE
end
function c71652522.filter(c)
	return c:IsFaceup() and c:IsRace(RACE_PSYCHO)
end
function c71652522.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	if chk==0 then return Duel.GetMatchingGroupCount(c71652522.filter,tp,LOCATION_MZONE,0,nil)==1 end
	local tc=Duel.GetMatchingGroup(c71652522.filter,tp,LOCATION_MZONE,0,nil):GetFirst()
	Duel.SetTargetCard(tc)
end
function c71652522.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) and tc:IsFaceup() then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_DIRECT_ATTACK)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+RESET_END)
		tc:RegisterEffect(e1)
		local e2=Effect.CreateEffect(e:GetHandler())
		e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		e2:SetCode(EVENT_PHASE+PHASE_BATTLE)
		e2:SetCountLimit(1)
		e2:SetReset(RESET_PHASE+PHASE_BATTLE)
		e2:SetOperation(c71652522.ctop)
		e2:SetLabelObject(tc)
		Duel.RegisterEffect(e2,tp)
		tc:RegisterFlagEffect(71652522,RESET_EVENT+0x1fc0000+RESET_PHASE+PHASE_BATTLE,0,1)
	end
end
function c71652522.ctop(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject()
	if tc:GetFlagEffect(71652522)~=0 and not Duel.GetControl(tc,1-tp) then
		if not tc:IsImmuneToEffect(e) and tc:IsAbleToChangeControler() then
			Duel.Destroy(tc,REASON_EFFECT)
		end
	end
end
