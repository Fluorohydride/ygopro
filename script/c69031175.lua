--BF－アーマード・ウィング
function c69031175.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,aux.FilterBoolFunction(Card.IsSetCard,0x33),aux.NonTuner(nil),1)
	c:EnableReviveLimit()
	--
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e1:SetValue(1)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_AVOID_BATTLE_DAMAGE)
	e2:SetValue(1)
	c:RegisterEffect(e2)
	--counter
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(69031175,0))
	e3:SetCategory(CATEGORY_COUNTER)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e3:SetCode(EVENT_DAMAGE_STEP_END)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCondition(c69031175.condition1)
	e3:SetOperation(c69031175.operation1)
	c:RegisterEffect(e3)
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(69031175,1))
	e4:SetCategory(CATEGORY_ATKCHANGE+CATEGORY_DEFCHANGE)
	e4:SetType(EFFECT_TYPE_IGNITION)
	e4:SetRange(LOCATION_MZONE)
	e4:SetCost(c69031175.cost2)
	e4:SetOperation(c69031175.operation2)
	c:RegisterEffect(e4)
end
function c69031175.condition1(e,tp,eg,ep,ev,re,r,rp)
	local atg=Duel.GetAttackTarget()
	return Duel.GetAttacker()==e:GetHandler() and atg and atg:IsRelateToBattle() and atg:GetCounter(0x2)==0
end
function c69031175.operation1(e,tp,eg,ep,ev,re,r,rp)
	local atg=Duel.GetAttackTarget()
	if atg:IsRelateToBattle() then
		atg:AddCounter(0x2,1)
	end
end
function c69031175.filter(c)
	return c:GetCounter(0x2)>0
end
function c69031175.cost2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c69031175.filter,tp,0,LOCATION_MZONE,1,nil) end
	local g=Duel.GetMatchingGroup(c69031175.filter,tp,0,LOCATION_MZONE,nil)
	local t=g:GetFirst()
	while t do
		t:RemoveCounter(tp,0x2,t:GetCounter(0x2),REASON_COST)
		t=g:GetNext()
	end
	Duel.SetTargetCard(g)
end
function c69031175.operation2(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	local t=g:GetFirst()
	while t do
		if t:IsRelateToEffect(e) then
			local e1=Effect.CreateEffect(c)
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_SET_ATTACK_FINAL)
			e1:SetValue(0)
			e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
			t:RegisterEffect(e1)
			local e2=Effect.CreateEffect(c)
			e2:SetType(EFFECT_TYPE_SINGLE)
			e2:SetCode(EFFECT_SET_DEFENCE_FINAL)
			e2:SetValue(0)
			e2:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
			t:RegisterEffect(e2)
		end
		t=g:GetNext()
	end
end
