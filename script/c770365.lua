--魔導皇聖 トリス
function c770365.initial_effect(c)
	--xyz summon
	aux.AddXyzProcedure(c,c770365.xyzfilter,2)
	c:EnableReviveLimit()
	--atklimit
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetRange(LOCATION_MZONE)
	e1:SetValue(c770365.atkval)
	c:RegisterEffect(e1)
	--confiem
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(770365,0))
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetCountLimit(1)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCost(c770365.cost)
	e2:SetTarget(c770365.target)
	e2:SetOperation(c770365.operation)
	c:RegisterEffect(e2)
end
function c770365.xyzfilter(c)
	return c:GetLevel()==5 and c:IsRace(RACE_SPELLCASTER)
end
function c770365.atkval(e,c)
	return Duel.GetOverlayCount(c:GetControler(),1,0)*300
end
function c770365.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
	e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c770365.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetFieldGroupCount(tp,LOCATION_DECK,0)>=5 end
end
function c770365.filter(c)
	return c:IsSetCard(0x106e)
end
function c770365.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.ShuffleDeck(tp)
	Duel.ConfirmDecktop(tp,5)
	local g=Duel.GetDecktopGroup(tp,5)
	local ct=g:FilterCount(c770365.filter,nil)
	local sg=Duel.GetMatchingGroup(Card.IsDestructable,tp,LOCATION_MZONE,LOCATION_MZONE,nil)
	if ct>0 and sg:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(770365,1)) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_DESTROY)
		local dg=sg:Select(tp,1,ct,nil)
		Duel.Destroy(dg,REASON_EFFECT)
	end
	Duel.SortDecktop(tp,tp,5)
end
