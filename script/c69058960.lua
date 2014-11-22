--No.13 ケインズ・デビル
function c69058960.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,nil,1,2)
	c:EnableReviveLimit()
	--pos
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(69058960,0))
	e1:SetCategory(CATEGORY_POSITION)
	e1:SetType(EFFECT_TYPE_QUICK_O)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCountLimit(1)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCost(c69058960.cost)
	e1:SetTarget(c69058960.target)
	e1:SetOperation(c69058960.operation)
	c:RegisterEffect(e1)
	--
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e2:SetCondition(c69058960.indcon)
	e2:SetValue(1)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	c:RegisterEffect(e3)
	--reflect
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE)
	e4:SetCode(EFFECT_REFLECT_BATTLE_DAMAGE)
	e4:SetCondition(c69058960.refcon)
	e4:SetValue(1)
	c:RegisterEffect(e4)
end
c69058960.xyz_number=13
function c69058960.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c69058960.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,0,LOCATION_MZONE)>0 end
end
function c69058960.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local g=Duel.GetFieldGroup(tp,0,LOCATION_MZONE)
	if g:GetCount()>0 then
		Duel.ChangePosition(g,POS_FACEUP_ATTACK)
	end
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_MUST_ATTACK)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(0,LOCATION_MZONE)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_MUST_BE_ATTACKED)
	e2:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
	c:RegisterEffect(e2)
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_CANNOT_EP)
	e3:SetRange(LOCATION_MZONE)
	e3:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e3:SetTargetRange(0,1)
	e3:SetCondition(c69058960.atcon)
	e3:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
	c:RegisterEffect(e3)
end
function c69058960.filter(c)
	return c:IsFaceup() and c:IsCode(95442074)
end
function c69058960.indcon(e)
	return Duel.IsExistingMatchingCard(c69058960.filter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil)
		and e:GetHandler():GetOverlayCount()~=0
end
function c69058960.refcon(e)
	return Duel.IsExistingMatchingCard(c69058960.filter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil)
		and Duel.GetAttackTarget()==e:GetHandler()
end
function c69058960.atcon(e)
	return Duel.IsExistingMatchingCard(Card.IsAttackable,e:GetHandlerPlayer(),0,LOCATION_MZONE,1,nil)
end
