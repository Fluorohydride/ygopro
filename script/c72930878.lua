--ブラック・ソニック
function c72930878.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_BE_BATTLE_TARGET)
	e1:SetCondition(c72930878.condition)
	e1:SetTarget(c72930878.target)
	e1:SetOperation(c72930878.activate)
	c:RegisterEffect(e1)
	--act in hand
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_TRAP_ACT_IN_HAND)
	e2:SetCondition(c72930878.handcon)
	c:RegisterEffect(e2)
end
function c72930878.handcon(e)
	return Duel.GetMatchingGroupCount(c72930878.cfilter,e:GetHandler():GetControler(),LOCATION_MZONE,0,nil)==3
end
function c72930878.cfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x33)
end
function c72930878.condition(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	return tc:IsControler(tp) and tc:IsFaceup() and tc:IsSetCard(0x33)
end
function c72930878.filter(c)
	return c:IsAttackPos() and c:IsAbleToRemove()
end
function c72930878.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c72930878.filter,tp,0,LOCATION_MZONE,1,nil) end
	local g=Duel.GetMatchingGroup(c72930878.filter,tp,0,LOCATION_MZONE,nil)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,g:GetCount(),0,0)
end
function c72930878.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c72930878.filter,tp,0,LOCATION_MZONE,nil)
	if g:GetCount()>0 then
		Duel.Remove(g,POS_FACEUP,REASON_EFFECT)
	end
end
