--カチコチドラゴン
function c69069911.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,aux.FilterEqualFunction(Card.GetLevel,4),2)
	c:EnableReviveLimit()
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(69069911,0))
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c69069911.atcon)
	e1:SetCost(c69069911.atcost)
	e1:SetOperation(c69069911.atop)
	c:RegisterEffect(e1)
end
function c69069911.atcon(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	local rc=tc:GetReasonCard()
	return eg:GetCount()==1 and rc==e:GetHandler() and rc:IsChainAttackable()
		and tc:IsType(TYPE_MONSTER) and tc:IsReason(REASON_BATTLE) and tc:IsLocation(LOCATION_GRAVE)
end
function c69069911.atcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c69069911.atop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToBattle() then
		Duel.ChainAttack()
	end
end
