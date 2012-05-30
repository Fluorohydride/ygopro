--栄誉の贄
function c82340056.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetCondition(c82340056.condition)
	e1:SetTarget(c82340056.target)
	e1:SetOperation(c82340056.activate)
	c:RegisterEffect(e1)
end
function c82340056.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetLP(tp)<=3000 and Duel.GetTurnPlayer()~=tp and Duel.GetAttackTarget()==nil
end
function c82340056.filter(c)
	return c:IsSetCard(0x21) and c:IsAbleToHand()
end
function c82340056.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>1
		and Duel.IsPlayerCanSpecialSummonMonster(tp,82340057,0,0x4011,0,0,1,RACE_ROCK,ATTRIBUTE_EARTH)
		and Duel.IsExistingMatchingCard(c82340056.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,2,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,2,0,0)
end
function c82340056.activate(e,tp,eg,ep,ev,re,r,rp)
	if not Duel.NegateAttack() then return end
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<2
		or not Duel.IsPlayerCanSpecialSummonMonster(tp,82340057,0,0x4011,0,0,1,RACE_ROCK,ATTRIBUTE_EARTH) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c82340056.filter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()==0 then return end
	Duel.BreakEffect()
	Duel.SendtoHand(g,nil,REASON_EFFECT)
	Duel.ConfirmCards(1-tp,g)
	Duel.ShuffleHand(tp)
	for i=1,2 do
		local token=Duel.CreateToken(tp,82340057)
		Duel.SpecialSummonStep(token,0,tp,tp,false,false,POS_FACEUP)
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UNRELEASABLE_NONSUM)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetValue(1)
		token:RegisterEffect(e1,true)
		local e2=Effect.CreateEffect(e:GetHandler())
		e2:SetType(EFFECT_TYPE_SINGLE)
		e2:SetCode(EFFECT_UNRELEASABLE_SUM)
		e2:SetReset(RESET_EVENT+0x1fe0000)
		e2:SetValue(c82340056.sumlimit)
		token:RegisterEffect(e2,true)
		local e3=Effect.CreateEffect(e:GetHandler())
		e3:SetType(EFFECT_TYPE_SINGLE)
		e3:SetCode(EFFECT_CANNOT_BE_SYNCHRO_MATERIAL)
		e3:SetReset(RESET_EVENT+0x1fe0000)
		e3:SetValue(1)
		token:RegisterEffect(e3,true)
	end
	Duel.SpecialSummonComplete()
end
function c82340056.sumlimit(e,c)
	return not c:IsSetCard(0x21)
end
