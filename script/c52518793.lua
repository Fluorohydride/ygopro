--剣闘獣の檻－コロッセウム
function c52518793.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--Add counter
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(52518793,0))
	e2:SetType(EFFECT_TYPE_TRIGGER_F+EFFECT_TYPE_FIELD)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCondition(c52518793.accon)
	e2:SetOperation(c52518793.acop)
	c:RegisterEffect(e2)
	--atkup
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_UPDATE_ATTACK)
	e3:SetRange(LOCATION_SZONE)
	e3:SetTargetRange(LOCATION_MZONE,0)
	e3:SetTarget(aux.TargetBoolFunction(Card.IsSetCard,0x19))
	e3:SetValue(c52518793.atkval)
	c:RegisterEffect(e3)
	--Destroy replace
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_SINGLE)
	e4:SetCode(EFFECT_DESTROY_REPLACE)
	e4:SetRange(LOCATION_SZONE)
	e4:SetTarget(c52518793.desreptg)
	e4:SetOperation(c52518793.desrepop)
	c:RegisterEffect(e4)
end
function c52518793.atkval(e,c)
	return e:GetHandler():GetCounter(0x7)*100
end
function c52518793.cfilter(c,tp)
	return c:GetPreviousLocation()==LOCATION_DECK
end
function c52518793.accon(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c52518793.cfilter,1,nil,tp)
end
function c52518793.acop(e,tp,eg,ep,ev,re,r,rp)
	e:GetHandler():AddCounter(0x7,1)
end
function c52518793.desreptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsCode,tp,LOCATION_HAND,0,1,nil,52518793) end
	return Duel.SelectYesNo(tp,aux.Stringid(52518793,1))
end
function c52518793.desrepop(e,tp,eg,ep,ev,re,r,rp)
	Duel.DiscardHand(tp,Card.IsCode,1,1,REASON_EFFECT+REASON_DISCARD,nil,52518793)
end
