--ゴーストリック・サキュバス
function c32224143.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,nil,2,2)
	c:EnableReviveLimit()
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(32224143,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCountLimit(1)
	e1:SetCost(c32224143.cost)
	e1:SetTarget(c32224143.target)
	e1:SetOperation(c32224143.operation)
	c:RegisterEffect(e1)
	--cannot be battle target
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
	e2:SetCondition(c32224143.atkcon)
	e2:SetValue(aux.imval1)
	c:RegisterEffect(e2)
end
function c32224143.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c32224143.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x8d)
end
function c32224143.filter(c,atk)
	return c:IsFaceup() and c:IsAttackBelow(atk) and c:IsDestructable()
end
function c32224143.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local cg=Duel.GetMatchingGroup(c32224143.cfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	local atk=cg:GetSum(Card.GetAttack)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c32224143.filter(chkc,atk) end
	if chk==0 then return Duel.IsExistingTarget(c32224143.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil,atk) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c32224143.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil,atk)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c32224143.operation(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local seq=tc:GetSequence()
	if tc:IsControler(1-tp) then seq=seq+16 end
	if tc:IsRelateToEffect(e) and tc:IsFaceup() and Duel.Destroy(tc,REASON_EFFECT)~=0 then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_FIELD)
		e1:SetCode(EFFECT_DISABLE_FIELD)
		e1:SetLabel(seq)
		e1:SetCondition(c32224143.discon)
		e1:SetOperation(c32224143.disop)
		e1:SetReset(0)
		Duel.RegisterEffect(e1,tp)
	end
end
function c32224143.discon(e)
	if Duel.IsExistingMatchingCard(c32224143.cfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil) then
		return true
	end
	e:Reset()
	return false
end
function c32224143.disop(e,tp)
	return bit.lshift(0x1,e:GetLabel())
end
function c32224143.atkcon(e)
	return Duel.IsExistingMatchingCard(c32224143.cfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,e:GetHandler())
end
