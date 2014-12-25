--BK チート・コミッショナー
function c76589815.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,nil,3,2)
	c:EnableReviveLimit()
	--must attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(0,LOCATION_MZONE)
	e1:SetCode(EFFECT_MUST_ATTACK)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCode(EFFECT_CANNOT_EP)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetTargetRange(0,1)
	e2:SetCondition(c76589815.becon)
	c:RegisterEffect(e2)
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
	e3:SetCondition(c76589815.atcon)
	e3:SetValue(1)
	c:RegisterEffect(e3)
	--atk
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(76589815,0))
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e3:SetCode(EVENT_ATTACK_ANNOUNCE)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCondition(c76589815.cfcon)
	e3:SetCost(c76589815.cfcost)
	e3:SetTarget(c76589815.cftg)
	e3:SetOperation(c76589815.cfop)
	c:RegisterEffect(e3)
end
function c76589815.becon(e)
	return Duel.IsExistingMatchingCard(Card.IsAttackable,e:GetHandlerPlayer(),0,LOCATION_MZONE,1,nil)
end
function c76589815.atfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x84)
end
function c76589815.atcon(e)
	return Duel.IsExistingMatchingCard(c76589815.atfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,e:GetHandler())
end
function c76589815.cfcon(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local at=Duel.GetAttackTarget()
	return (a:IsControler(tp) and a~=e:GetHandler() and a:IsSetCard(0x84))
		or (at and at:IsControler(tp) and at:IsFaceup() and at~=e:GetHandler() and at:IsSetCard(0x84))
end
function c76589815.cfcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,2,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,2,2,REASON_COST)
end
function c76589815.cftg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,0,LOCATION_HAND)>0
		and Duel.GetLocationCount(tp,LOCATION_SZONE)>0 end
end
function c76589815.cffilter(c)
	return c:IsType(TYPE_SPELL) and c:IsSSetable()
end
function c76589815.cfop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetFieldGroup(tp,0,LOCATION_HAND)
	if g:GetCount()==0 then return end
	Duel.ConfirmCards(tp,g)
	if Duel.GetLocationCount(tp,LOCATION_SZONE)>0 then
		local sg=g:Filter(c76589815.cffilter,nil)
		if sg:GetCount()>0 then
			Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SET)
			local setg=sg:Select(tp,1,1,nil)
			Duel.SSet(tp,setg:GetFirst())
			Duel.ConfirmCards(1-tp,setg)
		end
	end
	Duel.ShuffleHand(1-tp)
end
