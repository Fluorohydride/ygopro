--バトルマニア
function c31245780.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_POSITION)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_STANDBY_PHASE)
	e1:SetCondition(c31245780.condition)
	e1:SetTarget(c31245780.target)
	e1:SetOperation(c31245780.activate)
	c:RegisterEffect(e1)
end
function c31245780.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp and Duel.GetCurrentPhase()==PHASE_STANDBY
end
function c31245780.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsFaceup,tp,0,LOCATION_MZONE,1,nil) end
	local sg=Duel.GetMatchingGroup(Card.IsFaceup,tp,0,LOCATION_MZONE,nil)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,sg,sg:GetCount(),0,0)
end
function c31245780.activate(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local sg=Duel.GetMatchingGroup(Card.IsFaceup,tp,0,LOCATION_MZONE,nil)
	if sg:GetCount()>0 then
		Duel.ChangePosition(sg,POS_FACEUP_ATTACK,0,POS_FACEUP_ATTACK,0)
		local tc=sg:GetFirst()
		while tc do
			local e1=Effect.CreateEffect(c)
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_MUST_ATTACK)
			e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
			tc:RegisterEffect(e1)
			local e2=Effect.CreateEffect(c)
			e2:SetType(EFFECT_TYPE_SINGLE)
			e2:SetCode(EFFECT_CANNOT_CHANGE_POSITION)
			e2:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
			tc:RegisterEffect(e2)
			tc:RegisterFlagEffect(31245780,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,1)
			tc=sg:GetNext()
		end
		local be=Effect.CreateEffect(c)
		be:SetType(EFFECT_TYPE_FIELD)
		be:SetCode(EFFECT_CANNOT_EP)
		be:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
		be:SetTargetRange(0,1)
		be:SetCondition(c31245780.becon)
		be:SetReset(RESET_PHASE+PHASE_END)
		Duel.RegisterEffect(be,tp)
	end
end
function c31245780.befilter(c)
	return c:GetFlagEffect(31245780)~=0 and c:IsAttackable()
end
function c31245780.becon(e)
	return Duel.IsExistingMatchingCard(c31245780.befilter,e:GetHandlerPlayer(),0,LOCATION_MZONE,1,nil)
end
