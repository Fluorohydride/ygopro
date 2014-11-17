--星辉士 セイクリッド·ダイヤ
function c9272381.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,aux.XyzFilterFunctionF(c,aux.FilterBoolFunction(Card.IsAttribute,ATTRIBUTE_LIGHT),5),3,c9272381.ovfilter,aux.Stringid(9272381,0),5)
	c:EnableReviveLimit()
	--cannot sent to grave from deck
	local e1=Effect.CreateEffect(c) 
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_TO_GRAVE)
	e1:SetRange(LOCATION_MZONE) 
	e1:SetTargetRange(LOCATION_DECK,LOCATION_DECK)
	e1:Setcondition(c9272381.condition)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_CANNOT_DISCARD_DECK)
	e2:SetRange(LOCATION_MZONE)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)   
	e2:SetTargetRange(1,1)
	e2:Setcondition(c9272381.condition)
	c:RegisterEffect(e2)
	--discard
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCode(EFFECT_TO_HAND_REDIRECT)
	e3:SetTargetRange(LOCATION_GRAVE,LOCATION_GRAVE)
	e3:SetValue(LOCATION_REMOVED)
	e3:Setcondition(c9272381.condition)
	c:RegisterEffect(e3)
	--negate activate
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(9272381,1))
	e4:SetCategory(CATEGORY_NEGATE+CATEGORY_DESTROY)
	e4:SetType(EFFECT_TYPE_QUICK_O)
	e4:SetCode(EVENT_CHAINING)
	e4:SetRange(LOCATION_MZONE)
	e4:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DAMAGE_CAL)
	e4:Setcondition(c9272381.negcon)
	e4:SetCost(c9272381.negcost)
	e4:SetTarget(c9272381.negtg)
	e4:SetOperation(c9272381.negop)
	c:RegisterEffect(e4)
end 
function c9272381.ovfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x9c) and c:IsType(TYPE_XYZ) and not c:IsCode(9272381) and Duel.GetCurrentPhase()==PHASE_MAIN2
end
function c9272381.condition(e)
	return e:GetHandler():GetOverlayCount()>0
end
function c9272381.negcon(e,tp,eg,ep,ev,re,r,rp,chk)
	return re~=e and not e:GetHandler():IsStatus(STATUS_BATTLE_DESTROYED) and re:GetHandlerPlayer(1-tp) and re:IsActiveType(TYPE_MONSTER) and re:GetHandler():IsAttribute(ATTRIBUTE_DARK) and Duel.IsChainNegatable(ev)
end
function c9272381.negcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c9272381.negtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_NEGATE,eg,1,0,0)
	if re:GetHandler():IsDestructable() and re:GetHandler():IsRelateToEffect(re) then
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,1,0,0)
	end
end
function c9272381.negop(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateActivation(ev)
	if re:GetHandler():IsRelateToEffect(re) then
		Duel.Destroy(eg,REASON_EFFECT)
	end
end
