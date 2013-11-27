--召喚制限－猛突するモンスター
function c30834988.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_DRAW_PHASE)
	c:RegisterEffect(e1)
	--pos
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(30834988,0))
	e2:SetCategory(CATEGORY_POSITION)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTarget(c30834988.target)
	e2:SetOperation(c30834988.operation)
	c:RegisterEffect(e2)
	--
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_CANNOT_EP)
	e3:SetRange(LOCATION_SZONE)
	e3:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e3:SetTargetRange(1,1)
	e3:SetCondition(c30834988.epcon)
	c:RegisterEffect(e3)
end
function c30834988.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e) end
	Duel.SetTargetCard(eg)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,eg,eg:GetCount(),0,0)
end
function c30834988.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) then return end
	local g=eg:Filter(Card.IsRelateToEffect,nil,e)
	Duel.ChangePosition(g,POS_FACEUP_ATTACK)
	local tc=g:GetFirst()
	while tc do
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_MUST_ATTACK)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e1)
		tc=g:GetNext()
	end
	if Duel.GetFlagEffect(tp,30834988)==0 then
		Duel.RegisterFlagEffect(tp,30834988,RESET_PHASE+PHASE_END,0,1)
		e:SetLabel(0)
	else
		local count=e:GetLabel()
		e:SetLabel(count+1)
		if count>49 then
			Duel.SendtoGrave(c,REASON_RULE)
		end
	end
end
function c30834988.epfilter(c)
	return c:IsHasEffect(EFFECT_MUST_ATTACK) and c:IsAttackable()
end
function c30834988.epcon(e)
	return Duel.IsExistingMatchingCard(c30834988.epfilter,Duel.GetTurnPlayer(),LOCATION_MZONE,0,1,nil)
end
