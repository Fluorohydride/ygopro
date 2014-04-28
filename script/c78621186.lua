--バーバリアン・ハウリング
function c78621186.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(78621186,0))
	e1:SetCategory(CATEGORY_DAMAGE+CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_BE_BATTLE_TARGET)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCondition(c78621186.condition1)
	e1:SetTarget(c78621186.target)
	e1:SetOperation(c78621186.activate)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_BECOME_TARGET)
	e2:SetCondition(c78621186.condition2)
	c:RegisterEffect(e2)
end
function c78621186.condition1(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	return tc:IsFaceup() and tc:IsControler(tp) and tc:IsRace(RACE_WARRIOR)
end
function c78621186.cfilter(c,tp)
	return c:IsFaceup() and c:IsControler(tp) and c:IsLocation(LOCATION_MZONE) and c:IsRace(RACE_WARRIOR)
end
function c78621186.condition2(e,tp,eg,ep,ev,re,r,rp)
	return rp~=tp and re:IsActiveType(TYPE_MONSTER) and eg:IsExists(c78621186.cfilter,1,nil,tp)
end
function c78621186.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(1-tp) and chkc:IsFaceup() end
	if chk==0 then return Duel.IsExistingTarget(Card.IsFaceup,tp,0,LOCATION_MZONE,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FACEUP)
	local g=Duel.SelectTarget(tp,Card.IsFaceup,tp,0,LOCATION_MZONE,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,0)
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,g,1,0,0)
end
function c78621186.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		local dam=tc:GetBaseAttack()
		if dam<0 then dam=0 end
		if Duel.Damage(1-tp,dam,REASON_EFFECT)==0 then return end
		Duel.SendtoHand(tc,nil,REASON_EFFECT)
	end
end
