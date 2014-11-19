--竜星の気脈
function c43577607.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--atkup
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(LOCATION_MZONE,0)
	e2:SetTarget(c43577607.atktg)
	e2:SetValue(500)
	e2:SetCondition(c43577607.effcon)
	e2:SetLabel(2)
	c:RegisterEffect(e2)
	--replace
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_DESTROY_REPLACE)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCountLimit(1)
	e3:SetTarget(c43577607.reptg)
	e3:SetValue(c43577607.repval)
	e3:SetOperation(c43577607.repop)
	e3:SetCondition(c43577607.effcon)
	e3:SetLabel(3)
	c:RegisterEffect(e3)
	--cannot mset
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD)
	e4:SetCode(EFFECT_CANNOT_MSET)
	e4:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e4:SetRange(LOCATION_SZONE)
	e4:SetTargetRange(0,1)
	e4:SetCondition(c43577607.effcon)
	e4:SetLabel(4)
	c:RegisterEffect(e4)
	--poschange
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_FIELD)
	e5:SetCode(EFFECT_SET_POSITION)
	e5:SetRange(LOCATION_SZONE)
	e5:SetTargetRange(0,LOCATION_MZONE)
	e5:SetValue(POS_FACEUP_ATTACK)
	e5:SetCondition(c43577607.effcon)
	e5:SetLabel(4)
	c:RegisterEffect(e5)
	--destroy
	local e6=Effect.CreateEffect(c)
	e6:SetDescription(aux.Stringid(43577607,0))
	e6:SetCategory(CATEGORY_DESTROY)
	e6:SetType(EFFECT_TYPE_IGNITION)
	e6:SetRange(LOCATION_SZONE)
	e6:SetCondition(c43577607.effcon)
	e6:SetCost(c43577607.descost)
	e6:SetTarget(c43577607.destg)
	e6:SetOperation(c43577607.desop)
	e6:SetLabel(5)
	c:RegisterEffect(e6)
end
function c43577607.confilter(c)
	return c:IsSetCard(0x9e) and c:IsType(TYPE_MONSTER)
end
function c43577607.effcon(e)
	local g=Duel.GetMatchingGroup(c43577607.confilter,e:GetHandlerPlayer(),LOCATION_GRAVE,0,nil)
	return g:GetClassCount(Card.GetAttribute)>=e:GetLabel()
end
function c43577607.atktg(e,c)
	return c:IsSetCard(0x9e)
end
function c43577607.repfilter(c,tp)
	return c:IsFaceup() and c:IsSetCard(0x9e) and c:IsControler(tp) and c:IsLocation(LOCATION_MZONE)
end
function c43577607.reptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c43577607.repfilter,1,nil,tp) end
	return Duel.SelectYesNo(tp,aux.Stringid(43577607,1))
end
function c43577607.repval(e,c)
	return c43577607.repfilter(c,e:GetHandlerPlayer())
end
function c43577607.repop(e,tp,eg,ep,ev,re,r,rp)
	Duel.SendtoGrave(e:GetHandler(),REASON_EFFECT)
end
function c43577607.descost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToGraveAsCost() end
	Duel.SendtoGrave(e:GetHandler(),REASON_COST)
end
function c43577607.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,e:GetHandler()) end
	local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,g:GetCount(),0,0)
end
function c43577607.desop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,nil)
	if g:GetCount()>0 then
		Duel.Destroy(g,REASON_EFFECT)
	end
end
