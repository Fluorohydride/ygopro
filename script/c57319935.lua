--エクシーズ熱戦！！
function c57319935.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DAMAGE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c57319935.condition)
	e1:SetCost(c57319935.cost)
	e1:SetTarget(c57319935.target)
	e1:SetOperation(c57319935.activate)
	c:RegisterEffect(e1)
end
function c57319935.cfilter(c,tp)
	return c:IsType(TYPE_XYZ) and c:GetPreviousControler()==tp
end
function c57319935.condition(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	if c57319935.cfilter(tc,tp) then
		e:SetLabel(tc:GetRank())
		return true
	end
	tc=eg:GetNext()
	if tc and c57319935.cfilter(tc,tp) then
		e:SetLabel(tc:GetRank())
		return true
	end
	return false
end
function c57319935.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,1000) end
	Duel.PayLPCost(tp,1000)
end
function c57319935.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsRankBelow,tp,LOCATION_EXTRA,0,1,nil,e:GetLabel()) end
end
function c57319935.activate(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(57319935,0))
	local tc1=Duel.SelectMatchingCard(tp,Card.IsRankBelow,tp,LOCATION_EXTRA,0,1,1,nil,e:GetLabel()):GetFirst()
	Duel.Hint(HINT_SELECTMSG,1-tp,aux.Stringid(57319935,0))
	local tc2=Duel.SelectMatchingCard(1-tp,Card.IsRankBelow,1-tp,LOCATION_EXTRA,0,1,1,nil,e:GetLabel()):GetFirst()
	if tc1 then
		Duel.ConfirmCards(1-tp,tc1)
	end
	if tc2 then
		Duel.ConfirmCards(tp,tc2)
	end
	if tc1 and tc2 then
		local atk1=tc1:GetAttack()
		local atk2=tc2:GetAttack()
		if atk1>atk2 then
			Duel.Damage(1-tp,atk1-atk2,REASON_EFFECT)
		elseif atk1<atk2 then
			Duel.Damage(tp,atk2-atk1,REASON_EFFECT)
		end
	elseif tc1 then
		Duel.ConfirmCards(1-tp,tc1)
		local g=Duel.GetFieldGroup(1-tp,LOCATION_EXTRA,0)
		Duel.ConfirmCards(tp,g)
		Duel.Damage(1-tp,tc1:GetAttack(),REASON_EFFECT)
	elseif tc2 then
		Duel.ConfirmCards(tp,tc2)
		local g=Duel.GetFieldGroup(tp,LOCATION_EXTRA,0)
		Duel.ConfirmCards(1-tp,g)
		Duel.Damage(tp,tc2:GetAttack(),REASON_EFFECT)
	else
		local g1=Duel.GetFieldGroup(tp,LOCATION_EXTRA,0)
		local g2=Duel.GetFieldGroup(1-tp,LOCATION_EXTRA,0)
		Duel.ConfirmCards(tp,g2)
		Duel.ConfirmCards(1-tp,g1)
	end
end
