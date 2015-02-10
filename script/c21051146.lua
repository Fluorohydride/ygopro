--ブラッド・マジシャン－煉獄の魔術師－
function c21051146.initial_effect(c)
	c:EnableCounterPermit(0x3001)
	--add counter
	local e0=Effect.CreateEffect(c)
	e0:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e0:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
	e0:SetCode(EVENT_CHAINING)
	e0:SetRange(LOCATION_MZONE)
	e0:SetOperation(aux.chainreg)
	c:RegisterEffect(e0)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_FIELD)
	e1:SetCode(EVENT_CHAIN_SOLVED)
	e1:SetRange(LOCATION_MZONE)
	e1:SetOperation(c21051146.acop)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(21051146,0))
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_MZONE)
	e2:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e2:SetTarget(c21051146.destg)
	e2:SetOperation(c21051146.desop)
	c:RegisterEffect(e2)
end
function c21051146.acop(e,tp,eg,ep,ev,re,r,rp)
	if re:IsHasType(EFFECT_TYPE_ACTIVATE) and re:IsActiveType(TYPE_SPELL) and e:GetHandler():GetFlagEffect(1)>0 then
		e:GetHandler():AddCounter(0x3001,1)
	end
end
function c21051146.filter(c,cc,tp)
	local ct=math.ceil(c:GetAttack()/700)
	if ct==0 then ct=1 end
	return c:IsFaceup() and c:IsDestructable() and cc:IsCanRemoveCounter(tp,0x3001,ct,REASON_COST)
end
function c21051146.destg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and c21051146.filter(chkc,e:GetHandler(),tp) end
	if chk==0 then return Duel.IsExistingTarget(c21051146.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil,e:GetHandler(),tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
	local g=Duel.SelectTarget(tp,c21051146.filter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil,e:GetHandler(),tp)
	local ct=math.ceil(g:GetFirst():GetAttack()/700)
	if ct==0 then ct=1 end
	e:GetHandler():RemoveCounter(tp,0x3001,ct,REASON_COST)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,1,0,0)
end
function c21051146.desop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.Destroy(tc,REASON_EFFECT)
	end
end
