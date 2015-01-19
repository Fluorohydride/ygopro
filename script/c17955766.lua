--N・アクア・ドルフィン
function c17955766.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(17955766,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c17955766.cost)
	e1:SetTarget(c17955766.target)
	e1:SetOperation(c17955766.activate)
	c:RegisterEffect(e1)
end
function c17955766.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsDiscardable,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,Card.IsDiscardable,1,1,REASON_COST+REASON_DISCARD)
end
function c17955766.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,0,LOCATION_HAND)>0 end
end
function c17955766.filter(c,atk)
	return c:IsFaceup() and c:IsAttackAbove(atk)
end
function c17955766.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetFieldGroup(tp,0,LOCATION_HAND)
	if g:GetCount()>0 then
		Duel.ConfirmCards(tp,g)
		Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(17955766,1))
		local tg=g:FilterSelect(tp,Card.IsType,1,1,nil,TYPE_MONSTER)
		local tc=tg:GetFirst()
		if tc then
			local atk=tc:GetAttack()
			if atk>=0 and Duel.IsExistingMatchingCard(c17955766.filter,tp,LOCATION_MZONE,0,1,nil,atk) then
				Duel.Destroy(tc,REASON_EFFECT)
				Duel.Damage(1-tp,500,REASON_EFFECT)
			else
				Duel.Damage(tp,500,REASON_EFFECT)
			end
		end
		Duel.ShuffleHand(1-tp)
	end
end
