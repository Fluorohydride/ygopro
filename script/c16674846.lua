--スピリット・フォース
function c16674846.initial_effect(c)
	--no damage
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_PRE_BATTLE_DAMAGE)
	e1:SetCondition(c16674846.condition)
	e1:SetOperation(c16674846.operation)
	c:RegisterEffect(e1)
end
function c16674846.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnPlayer()~=tp and ep==tp
end
function c16674846.filter(c)
	return c:IsDefenceBelow(1500) and c:IsType(TYPE_TUNER) and c:IsRace(RACE_WARRIOR)
		and c:IsAbleToHand() and not c:IsHasEffect(EFFECT_NECRO_VALLEY)
end
function c16674846.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.ChangeBattleDamage(ep,0)
	local g=Duel.GetMatchingGroup(c16674846.filter,tp,LOCATION_GRAVE,0,nil)
	if g:GetCount()~=0 and Duel.SelectYesNo(tp,aux.Stringid(16674846,0)) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local sg=g:Select(tp,1,1,nil)
		Duel.SendtoHand(sg,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,sg)
	end
end
