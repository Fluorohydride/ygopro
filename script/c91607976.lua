--D・クロックン
function c91607976.initial_effect(c)
	--atk
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetCondition(c91607976.cona)
	e1:SetValue(c91607976.vala)
	c:RegisterEffect(e1)
	--counter
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(91607976,0))
	e2:SetCategory(CATEGORY_COUNTER)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetCountLimit(1)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCondition(c91607976.cond1)
	e2:SetOperation(c91607976.opd1)
	c:RegisterEffect(e2)
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(91607976,1))
	e3:SetCategory(CATEGORY_DAMAGE)
	e3:SetType(EFFECT_TYPE_IGNITION)
	e3:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e3:SetCountLimit(1)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCondition(c91607976.cond2)
	e3:SetCost(c91607976.costd2)
	e3:SetTarget(c91607976.tgd2)
	e3:SetOperation(c91607976.opd2)
	c:RegisterEffect(e3)
end
function c91607976.cona(e)
	return e:GetHandler():IsAttackPos()
end
function c91607976.vala(e,c)
	return c:GetCounter(0x8)*500
end
function c91607976.cond1(e,tp,eg,ep,ev,re,r,rp)
	return not e:GetHandler():IsDisabled() and e:GetHandler():IsDefencePos()
end
function c91607976.opd1(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		e:GetHandler():AddCounter(0x8,1)
	end
end
function c91607976.cond2(e,tp,eg,ep,ev,re,r,rp)
	return not e:GetHandler():IsDisabled() and e:GetHandler():IsDefencePos()
end
function c91607976.costd2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleasable() end
	e:SetLabel(e:GetHandler():GetCounter(0x8)*1000)
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c91607976.tgd2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():GetCounter(0x8)>0 end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(e:GetLabel())
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,e:GetLabel())
end
function c91607976.opd2(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
