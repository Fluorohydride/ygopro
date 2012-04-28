--闇の住人シャドウキラー
function c20939559.initial_effect(c)
	--direct attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_DIRECT_ATTACK)
	e1:SetCondition(c20939559.con)
	c:RegisterEffect(e1)
end
function c20939559.con(e)
	return not Duel.IsExistingMatchingCard(Card.IsAttackPos,e:GetHandler():GetControler(),0,LOCATION_MZONE,1,nil)
end
