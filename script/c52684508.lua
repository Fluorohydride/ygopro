--黒炎弾
function c52684508.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c52684508.cost)
	e1:SetTarget(c52684508.target)
	e1:SetOperation(c52684508.activate)
	c:RegisterEffect(e1)
	if not c52684508.global_check then
		c52684508.global_check=true
		c52684508[0]=true
		c52684508[1]=true
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_ATTACK_ANNOUNCE)
		ge1:SetOperation(c52684508.checkop)
		Duel.RegisterEffect(ge1,0)
		local ge2=Effect.CreateEffect(c)
		ge2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge2:SetCode(EVENT_PHASE_START+PHASE_DRAW)
		ge2:SetOperation(c52684508.clear)
		Duel.RegisterEffect(ge2,0)
	end
end
function c52684508.checkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	if tc:IsCode(74677422) then
		c52684508[tc:GetControler()]=false
	end
end
function c52684508.clear(e,tp,eg,ep,ev,re,r,rp)
	c52684508[0]=true
	c52684508[1]=true
end
function c52684508.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return c52684508[tp] end
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_CANNOT_ATTACK_ANNOUNCE)
	e1:SetProperty(EFFECT_FLAG_OATH)
	e1:SetTarget(aux.TargetBoolFunction(Card.IsCode,74677422))
	e1:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e1:SetReset(RESET_PHASE+RESET_END)
	Duel.RegisterEffect(e1,tp)
end
function c52684508.filter(c)
	return c:IsFaceup() and c:IsCode(74677422)
end
function c52684508.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsControler(tp) and chkc:IsLocation(LOCATION_MZONE) and c52684508.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c52684508.filter,tp,LOCATION_MZONE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	local g=Duel.SelectTarget(tp,c52684508.filter,tp,LOCATION_MZONE,0,1,1,nil)
end
function c52684508.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc and tc:IsFaceup() and tc:IsRelateToEffect(e) then
		Duel.Damage(1-tp,tc:GetBaseAttack(),REASON_EFFECT)
	end
end
